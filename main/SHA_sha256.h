#pragma once
#include <stdint.h>

#define HASH_LENGTH 32
#define BLOCK_LENGTH 64

#ifdef __cplusplus
extern "C"
{
#endif
void createHash(const uint8_t*,int, uint8_t*);
#ifdef __cplusplus
}
#endif