#include "xtea.h"

const uint32_t xtea_delta = 0x61C88647;

void xtea_decrypt_block(uint32_t block[2], uint32_t key[4]) {
    uint32_t slice_1 = block[0];
    uint32_t slice_2 = block[1];
    int32_t sum = 0xC6EF3720;
    for(size_t i = 0; i < 32; i++) {
        slice_2 -= (((slice_1 >> 0x5) + key[3]) ^ ((slice_1 << 0x4) + key[2])) ^ (sum + slice_1);
        slice_1 -= (((slice_2 << 0x4) + key[0]) ^ ((slice_2 >> 0x5) + key[1])) ^ (sum + slice_2);
        sum += xtea_delta;
    }
    block[0] = slice_1;
    block[1] = slice_2;
}

void xtea_decrypt(size_t data_size, unsigned char *data, uint32_t key[4]) {
    if(data_size >= 8) {
        if(data_size % 8) {
            xtea_decrypt_block((uint32_t *)(data + data_size - 8), key);
        }
        for(size_t i = 0; i < data_size / 8; i++) {
            xtea_decrypt_block((uint32_t *)(data + i * 8), key);
        }
    }
}

void xtea_encrypt_block(uint32_t block[2], uint32_t key[4]) {
    uint32_t slice_1 = block[0];
    uint32_t slice_2 = block[1];
    int32_t sum = 0;
    for(size_t i = 0; i < 32; i++) {
        sum = (uint64_t)sum - xtea_delta;
        slice_1 += (((slice_2 << 0x4) + key[0]) ^ ((slice_2 >> 0x5) + key[1])) ^ (sum + slice_2);
        slice_2 += (((slice_1 >> 0x5) + key[3]) ^ ((slice_1 << 0x4) + key[2])) ^ (sum + slice_1);
    }
    block[0] = slice_1;
    block[1] = slice_2;
}

void xtea_encrypt(size_t data_size, unsigned char *data, uint32_t key[4]) {
    if(data_size >= 8) {
        for(size_t i = 0; i < data_size / 8; i++) {
            xtea_encrypt_block((uint32_t *)(data + i * 8), key);
        }
        if(data_size % 8) {
            xtea_encrypt_block((uint32_t *)(data + data_size - 8), key);
        }
    }
}

