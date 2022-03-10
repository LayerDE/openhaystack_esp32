#pragma once

#include "uECC.h"

class uECC{
    public:
        uECC();
        uECC(uECC_RNG_Function rng);
        uECC(uECC_RNG_Function rng, uECC_Curve curve);
        ~uECC();
        bool sign(const uint8_t* private_key,
            const uint8_t* message_hash,
            unsigned hash_size,
            uint8_t* signature);
        bool verify(const uint8_t* public_key,
            const uint8_t* message_hash,
            unsigned hash_size,
            const uint8_t* signature);
        void crypt();
        void encrypt();
        int private_key_size();
        int public_key_size();
        int compressed_public_key_size();
        bool make_keys(uint8_t* public_key, uint8_t* private_key);
        void compress(const uint8_t* public_key, uint8_t* compressed);
        void decompress(const uint8_t* compressed, uint8_t* public_key);
        bool valid_public_key(const uint8_t* public_key);
        bool compute_public_key(const uint8_t* private_key, uint8_t* public_key);

    private:
        uECC_Curve g_curve;
        uECC_RNG_Function g_rng_function;
};