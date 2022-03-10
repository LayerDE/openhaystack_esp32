#include <stdlib.h>
#include <string.h>
#include "uECC.h"
#include "uECC-handle.hpp"

bool uECC::verify(const uint8_t* public_key,
        const uint8_t* message_hash,
        unsigned hash_size,
        const uint8_t* signature){
    return uECC_verify(public_key,message_hash,hash_size,signature,g_curve);
}

bool uECC::sign(const uint8_t* private_key,
        const uint8_t* message_hash,
        unsigned hash_size,
        uint8_t* signature){

    return uECC_sign(private_key,message_hash,hash_size,signature,g_curve,g_rng_function);
}

uECC::uECC(uECC_RNG_Function rng) : uECC(rng, uECC_secp160r1()){
}

uECC::uECC() : uECC(0){
}

uECC::uECC(uECC_RNG_Function rng, uECC_Curve curve){
    g_rng_function = rng;
    g_curve = curve;
}

uECC::~uECC(){
}

int uECC::private_key_size(){
    return  uECC_curve_private_key_size(g_curve);
}
int uECC::public_key_size(){
    return uECC_curve_public_key_size(g_curve);
}

int uECC::compressed_public_key_size(){
    return uECC_curve_private_key_size(g_curve)+1;
}

bool uECC::make_keys(uint8_t* public_key, uint8_t* private_key){
    return uECC_make_key(public_key,private_key,g_curve,g_rng_function);
}

void uECC::compress(const uint8_t* public_key, uint8_t* compressed){
    uECC_compress(public_key, compressed,g_curve);
}

void uECC::decompress(const uint8_t* compressed, uint8_t* public_key){
    uECC_decompress(compressed, public_key, g_curve);
}

bool uECC::valid_public_key(const uint8_t* public_key){
    return uECC_valid_public_key(public_key,g_curve);
}

bool uECC::compute_public_key(const uint8_t* private_key, uint8_t* public_key){
    return uECC_compute_public_key(private_key,public_key,g_curve);
}
