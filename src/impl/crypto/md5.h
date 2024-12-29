#ifndef RINGWORLD__CRYPTO__MD5_H
#define RINGWORLD__CRYPTO__MD5_H

#include <stddef.h>

/**
 * Generates an MD5 hash of the given data
 * @param data      data to create a hash of
 * @param data_size size of data
 * @param hash      string buffer to store the hash in; must be at least 32 bytes long
 */
void generate_md5_hash(char *data, size_t data_size, char *hash);

#endif