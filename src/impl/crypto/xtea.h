#ifndef RINGWORLD__CRYPTO__XTEA_H
#define RINGWORLD__CRYPTO__XTEA_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * Decrypt data using XTEA
 * @param data_size size of data
 * @param data      data to decrypt
 * @param key       key to use for decryption
 */
void xtea_decrypt(size_t data_size, unsigned char *data, uint32_t *key);

#ifdef __cplusplus
}
#endif

#endif