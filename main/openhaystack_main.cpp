#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

#include <nvs_flash.h>
#include <esp_partition.h>

#include <esp_bt.h>
#include <esp_gap_ble_api.h>
#include <esp_gattc_api.h>
#include <esp_gatt_defs.h>
#include <esp_bt_main.h>
#include <esp_bt_defs.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <esp_random.h>     

#include "data.hpp"
#include "SHA_sha256.h"
#include "uECC-handle.hpp"


static const char* LOG_TAG = "open_haystack";

static uint8_t private_key_init[28];
static uint8_t private_key[28];
static uint8_t public_key[28];

static uECC handler;

static int RNG_sha256(uint8_t *dest, unsigned size) {
  if(size == 0)
    return 1;
  const int blocksize = 32;
  int blocks = size / blocksize + (size % blocksize != 0);
  data temp = data(blocks * blocksize);
  esp_fill_random((uint8_t*)temp.get_data(), blocks*blocksize);
  for(int x = 0; x < blocks - 1; x++)
    createHash(temp.get_data()+x*blocksize,blocksize,dest+x*blocksize);
  data end = data(blocksize);
  createHash(temp.get_data(), blocksize, (uint8_t*)end.get_data());
  memcpy(dest+blocks*blocksize - 1, end.get_data(),size % blocksize == 0 ? blocksize: size % blocksize);
  return 1;
}

/** Callback function for BT events */
static void esp_gap_cb(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param);

/** Random device address */
static esp_bd_addr_t rnd_addr = { 0xFF, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF };

/** Advertisement payload */
static uint8_t adv_data[31] = {
	0x1e, /* Length (30) */
	0xff, /* Manufacturer Specific Data (type 0xff) */
	0x4c, 0x00, /* Company ID (Apple) */
	0x12, 0x19, /* Offline Finding type and length */
	0x00, /* State */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, /* First two bits */
	0x00, /* Hint (0x00) */
};

/* https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/bluetooth/esp_gap_ble.html#_CPPv420esp_ble_adv_params_t */
static esp_ble_adv_params_t ble_adv_params = {
    // Advertising min interval:
    // Minimum advertising interval for undirected and low duty cycle
    // directed advertising. Range: 0x0020 to 0x4000 Default: N = 0x0800
    // (1.28 second) Time = N * 0.625 msec Time Range: 20 ms to 10.24 sec
    .adv_int_min        = 0x0640, // 1s
    // Advertising max interval:
    // Maximum advertising interval for undirected and low duty cycle
    // directed advertising. Range: 0x0020 to 0x4000 Default: N = 0x0800
    // (1.28 second) Time = N * 0.625 msec Time Range: 20 ms to 10.24 sec
    .adv_int_max        = 0x0C80, // 2s
    // Advertisement type
    .adv_type           = ADV_TYPE_NONCONN_IND,
    // Use the random address
    .own_addr_type      = BLE_ADDR_TYPE_RANDOM,
    // All channels
    .channel_map        = ADV_CHNL_ALL,
    // Allow both scan and connection requests from anyone. 
    .adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};

static void esp_gap_cb(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
    esp_err_t err;

    switch (event) {
        case ESP_GAP_BLE_ADV_DATA_RAW_SET_COMPLETE_EVT:
            esp_ble_gap_start_advertising(&ble_adv_params);
            break;

        case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:
            //adv start complete event to indicate adv start successfully or failed
            if ((err = param->adv_start_cmpl.status) != ESP_BT_STATUS_SUCCESS) {
                ESP_LOGE(LOG_TAG, "advertising start failed: %s", esp_err_to_name(err));
            } else {
                ESP_LOGI(LOG_TAG, "advertising has started.");
            }
            break;

        case ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT:
            if ((err = param->adv_stop_cmpl.status) != ESP_BT_STATUS_SUCCESS){
                ESP_LOGE(LOG_TAG, "adv stop failed: %s", esp_err_to_name(err));
            }
            else {
                ESP_LOGI(LOG_TAG, "stop adv successfully");
            }
            break;
        default:
            break;
    }
}

void load_key() {
    uint8_t keys[28*2];
    memcpy(keys,private_key_init,28);
    memcpy(&keys[28],private_key,28);
    uint8_t hash[32];
    createHash(keys, 56, hash);
    memcpy(private_key,&hash[0],28);
    handler.compute_public_key(private_key,public_key);
}

int init_key(uint8_t *dst, size_t size){
    const esp_partition_t *keypart = esp_partition_find_first((esp_partition_type_t)0x40, (esp_partition_subtype_t)0x00, "key");
    if (keypart == NULL) {
        ESP_LOGE(LOG_TAG, "Could not find key partition");
        return 1;
    }
    esp_err_t status;
    status = esp_partition_read(keypart, 0, dst, size);
    if (status != ESP_OK) {
        ESP_LOGE(LOG_TAG, "Could not read key from partition: %s", esp_err_to_name(status));
    }
    load_key();
    return (int)status;
}

void set_addr_from_key(esp_bd_addr_t addr, uint8_t *public_key) {
	addr[0] = public_key[0] | 0b11000000;
	addr[1] = public_key[1];
	addr[2] = public_key[2];
	addr[3] = public_key[3];
	addr[4] = public_key[4];
	addr[5] = public_key[5];
}

void set_payload_from_key(uint8_t *payload, uint8_t *public_key) {
    /* copy last 22 bytes */
	memcpy(&payload[7], &public_key[6], 22);
	/* append two bits of public key */
	payload[29] = public_key[0] >> 6;
}

extern "C" void app_main(void)
{
    handler = uECC(RNG_sha256,uECC_secp224r1());
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    esp_bt_controller_init(&bt_cfg);
    esp_bt_controller_enable(ESP_BT_MODE_BLE);

    esp_bluedroid_init();
    esp_bluedroid_enable();

    // Load the public key from the key partition

    if (init_key(private_key_init, sizeof(private_key_init)) != ESP_OK) {
        ESP_LOGE(LOG_TAG, "Could not read the key, stopping.");
        return;
    }

    set_addr_from_key(rnd_addr, public_key);
    set_payload_from_key(adv_data, public_key);

    ESP_LOGI(LOG_TAG, "using device address: %02x %02x %02x %02x %02x %02x", rnd_addr[0], rnd_addr[1], rnd_addr[2], rnd_addr[3], rnd_addr[4], rnd_addr[5]);

    esp_err_t status;
    //register the scan callback function to the gap module
    if ((status = (esp_err_t)esp_gap_cb) != ESP_OK) {
        ESP_LOGE(LOG_TAG, "gap register error: %s", esp_err_to_name(status));
        return;
    }

    if ((status = esp_ble_gap_set_rand_addr(rnd_addr)) != ESP_OK) {
        ESP_LOGE(LOG_TAG, "couldn't set random address: %s", esp_err_to_name(status));
        return;
    }
    if ((esp_ble_gap_config_adv_data_raw((uint8_t*)&adv_data, sizeof(adv_data))) != ESP_OK) {
        ESP_LOGE(LOG_TAG, "couldn't configure BLE adv: %s", esp_err_to_name(status));
        return;
    }
    ESP_LOGI(LOG_TAG, "application initialized");
}
