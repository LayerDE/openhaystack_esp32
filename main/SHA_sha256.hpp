#pragma once

#ifndef Sha256_h
#define Sha256_h    

#include <stdint.h>
#include <inttypes.h>
#include "SHA_sha256.h"

class Sha256 {

    union Buffer {
        uint8_t b[BLOCK_LENGTH];
        uint32_t w[BLOCK_LENGTH / 4];
    };

    union State {
        uint8_t b[HASH_LENGTH];
        uint32_t w[HASH_LENGTH / 4];
    };

public:
    Sha256();
    Sha256(const uint8_t* key, size_t keyLength);

    // Reset to initial state, but preserve key material.
    void reset(void);
    void quick_hash(const uint8_t* in, int in_lenght, uint8_t* out);
    uint8_t* result(void);
    uint8_t* resultHmac(void);
#if defined(ARDUINO) && ARDUINO >= 100
    virtual size_t write(uint8_t);
#else
    virtual void write(uint8_t);
#endif

private:
    void init();
    void hashBlock();
    void padBlock();
    void push(uint8_t data);

    uint32_t byteCount;

    uint8_t keyBuffer[BLOCK_LENGTH];
    uint8_t innerHash[HASH_LENGTH];

    State state;
    Buffer buffer;
    uint8_t bufferOffset;
};

#endif
