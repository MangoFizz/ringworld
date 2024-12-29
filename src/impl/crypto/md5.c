/**
 * This code implements the MD5 Algorithm defined in RFC 1321. I was
 * suffering while trying to implement the transform routine until 
 * I found the reference implementation in RFC 1321, which is what
 * the thing is based on. 
 * 
 * The code of this file is derived from the text of the RFC and its
 * reference implementation. The original code is Copyright (C) 1992 
 * by RSA Data Security, Inc. as you can see in the license included 
 * below. 
 * 
 * Its text is available at http://www.ietf.org/rfc/rfc1321.txt
*/

/**
 * Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All
 * rights reserved.
 * 
 * License to copy and use this software is granted provided that it
 * is identified as the "RSA Data Security, Inc. MD5 Message-Digest
 * Algorithm" in all material mentioning or referencing this software
 * or this function.
 * 
 * License is also granted to make and use derivative works provided
 * that such works are identified as "derived from the RSA Data
 * Security, Inc. MD5 Message-Digest Algorithm" in all material
 * mentioning or referencing the derived work.
 * 
 * RSA Data Security, Inc. makes no representations concerning either
 * the merchantability of this software or the suitability of this
 * software for any particular purpose. It is provided "as is"
 * without express or implied warranty of any kind.
 * 
 * These notices must be retained in any copies of any part of this
 * documentation and/or software.
*/

#include <stdint.h>
#include <stdio.h>

typedef struct md5_context_t {
    uint32_t count[2];
    uint32_t state[4];
    char buffer[64];
} md5_context_t;

/**
 * Constants for md5_transform routine.
 */
#define S11 7
#define S12 12
#define S13 17
#define S14 22
#define S21 5
#define S22 9
#define S23 14
#define S24 20
#define S31 4
#define S32 11
#define S33 16
#define S34 23
#define S41 6
#define S42 10
#define S43 15
#define S44 21

/**
 * F, G, H and I are basic MD5 functions.
 */
#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z)))

/**
 * ROTATE_LEFT rotates x left n bits.
 */
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))

/**
 * FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4.
 * Rotation is separate from addition to prevent recomputation.
 */
#define FF(a, b, c, d, x, s, ac) { \
    (a) += F ((b), (c), (d)) + (x) + (uint32_t)(ac); \
    (a) = ROTATE_LEFT ((a), (s)); \
    (a) += (b); \
}
#define GG(a, b, c, d, x, s, ac) { \
    (a) += G ((b), (c), (d)) + (x) + (uint32_t)(ac); \
    (a) = ROTATE_LEFT ((a), (s)); \
    (a) += (b); \
}
#define HH(a, b, c, d, x, s, ac) { \
    (a) += H ((b), (c), (d)) + (x) + (uint32_t)(ac); \
    (a) = ROTATE_LEFT ((a), (s)); \
    (a) += (b); \
}
#define II(a, b, c, d, x, s, ac) { \
    (a) += I ((b), (c), (d)) + (x) + (uint32_t)(ac); \
    (a) = ROTATE_LEFT ((a), (s)); \
    (a) += (b); \
}

/**
 * MD5 basic transformation. Transforms state based on block.
 */
void md5_transform(md5_context_t *context, unsigned char block[64]) {
    uint32_t *state = context->state;
    uint32_t a = state[0], b = state[1], c = state[2], d = state[3], x[16];

    for(size_t i = 0, j = 0; j < 64; i++, j += 4) {
        x[i] = ((uint32_t)block[j]) | (((uint32_t)block[j + 1]) << 8) | (((uint32_t)block[j + 2]) << 16) | (((uint32_t)block[j + 3]) << 24);
    }

    /* Round 1 */
    FF(a, b, c, d, x[0], S11, 0xd76aa478);
    FF(d, a, b, c, x[1], S12, 0xe8c7b756);
    FF(c, d, a, b, x[2], S13, 0x242070db);
    FF(b, c, d, a, x[3], S14, 0xc1bdceee);
    FF(a, b, c, d, x[4], S11, 0xf57c0faf);
    FF(d, a, b, c, x[5], S12, 0x4787c62a);
    FF(c, d, a, b, x[6], S13, 0xa8304613);
    FF(b, c, d, a, x[7], S14, 0xfd469501);
    FF(a, b, c, d, x[8], S11, 0x698098d8);
    FF(d, a, b, c, x[9], S12, 0x8b44f7af);
    FF(c, d, a, b, x[10], S13, 0xffff5bb1);
    FF(b, c, d, a, x[11], S14, 0x895cd7be);
    FF(a, b, c, d, x[12], S11, 0x6b901122);
    FF(d, a, b, c, x[13], S12, 0xfd987193);
    FF(c, d, a, b, x[14], S13, 0xa679438e);
    FF(b, c, d, a, x[15], S14, 0x49b40821);

    /* Round 2 */
    GG(a, b, c, d, x[1], S21, 0xf61e2562);
    GG(d, a, b, c, x[6], S22, 0xc040b340);
    GG(c, d, a, b, x[11], S23, 0x265e5a51);
    GG(b, c, d, a, x[0], S24, 0xe9b6c7aa);
    GG(a, b, c, d, x[5], S21, 0xd62f105d);
    GG(d, a, b, c, x[10], S22,  0x2441453);
    GG(c, d, a, b, x[15], S23, 0xd8a1e681);
    GG(b, c, d, a, x[4], S24, 0xe7d3fbc8);
    GG(a, b, c, d, x[9], S21, 0x21e1cde6);
    GG(d, a, b, c, x[14], S22, 0xc33707d6);
    GG(c, d, a, b, x[3], S23, 0xf4d50d87);
    GG(b, c, d, a, x[8], S24, 0x455a14ed);
    GG(a, b, c, d, x[13], S21, 0xa9e3e905);
    GG(d, a, b, c, x[2], S22, 0xfcefa3f8);
    GG(c, d, a, b, x[7], S23, 0x676f02d9);
    GG(b, c, d, a, x[12], S24, 0x8d2a4c8a);

    /* Round 3 */
    HH(a, b, c, d, x[5], S31, 0xfffa3942);
    HH(d, a, b, c, x[8], S32, 0x8771f681);
    HH(c, d, a, b, x[11], S33, 0x6d9d6122);
    HH(b, c, d, a, x[14], S34, 0xfde5380c);
    HH(a, b, c, d, x[1], S31, 0xa4beea44);
    HH(d, a, b, c, x[4], S32, 0x4bdecfa9);
    HH(c, d, a, b, x[7], S33, 0xf6bb4b60);
    HH(b, c, d, a, x[10], S34, 0xbebfbc70);
    HH(a, b, c, d, x[13], S31, 0x289b7ec6);
    HH(d, a, b, c, x[0], S32, 0xeaa127fa);
    HH(c, d, a, b, x[3], S33, 0xd4ef3085);
    HH(b, c, d, a, x[6], S34, 0x04881d05);
    HH(a, b, c, d, x[9], S31, 0xd9d4d039);
    HH(d, a, b, c, x[12], S32, 0xe6db99e5);
    HH(c, d, a, b, x[15], S33, 0x1fa27cf8);
    HH(b, c, d, a, x[2], S34, 0xc4ac5665);

    /* Round 4 */
    II(a, b, c, d, x[0], S41, 0xf4292244);
    II(d, a, b, c, x[7], S42, 0x432aff97);
    II(c, d, a, b, x[14], S43, 0xab9423a7);
    II(b, c, d, a, x[5], S44, 0xfc93a039);
    II(a, b, c, d, x[12], S41, 0x655b59c3);
    II(d, a, b, c, x[3], S42, 0x8f0ccc92);
    II(c, d, a, b, x[10], S43, 0xffeff47d);
    II(b, c, d, a, x[1], S44, 0x85845dd1);
    II(a, b, c, d, x[8], S41, 0x6fa87e4f);
    II(d, a, b, c, x[15], S42, 0xfe2ce6e0);
    II(c, d, a, b, x[6], S43, 0xa3014314);
    II(b, c, d, a, x[13], S44, 0x4e0811a1);
    II(a, b, c, d, x[4], S41, 0xf7537e82);
    II(d, a, b, c, x[11], S42, 0xbd3af235);
    II(c, d, a, b, x[2], S43, 0x2ad7d2bb);
    II(b, c, d, a, x[9], S44, 0xeb86d391);

    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;
}

/**
 * MD5 block update operation. Continues an MD5 message-digest
 * operation, processing another message block, and updating the
 * context.
 */
void md5_update(md5_context_t *context, char *data, size_t data_size) {
    uint32_t part_len = context->count[0] + data_size * 8;
    uint32_t index = (uint32_t)((context->count[0] / 8) & 0x3F);
    context->count[0] = part_len;
    if(part_len < data_size * 8) {
        context->count[1]++;
    }
    part_len = 64 - index;
    context->count[1] += data_size >> 29;
    if(data_size < part_len) {
        part_len = 0;
    }
    else {
        char *output = context->buffer + index;
        for(size_t i = 0; i < part_len; i++) {
            output[i] = data[i];
        }
        md5_transform(context, context->buffer);
        for(size_t i = part_len; i + 63 < data_size; i += 64) {
            md5_transform(context, data + i);
            part_len += 64;
        }
        index = 0;
    }
    char *output = context->buffer + index;
    for(size_t i = 0; i < data_size - part_len; i++) {
        output[i] = data[i + part_len];
    }
}

/** 
 * MD5 finalization. Ends an MD5 message-digest operation, writing the 
 * the message digest and zeroizing the context.
 */
void md5_final(unsigned char hash[16], md5_context_t *context) {
    uint8_t bits[8];

    for(size_t i = 0, j = 0; j < 8; i++, j += 4) {
        bits[j] = (uint8_t)(context->count[i] & 0xFF);
        bits[j + 1] = (uint8_t)((context->count[i] >> 8) & 0xFF);
        bits[j + 2] = (uint8_t)((context->count[i] >> 16) & 0xFF);
        bits[j + 3] = (uint8_t)((context->count[i] >> 24) & 0xFF);
    }

    uint32_t index = (uint32_t)((context->count[0] / 8) & 0x3F);
    uint32_t pad_len = (index < 56) ? (56 - index) : (120 - index);
    char padding[64];
    padding[0] = 0x80;
    for(size_t i = 1; i < sizeof(padding); i++) {
        padding[i] = 0;
    }

    md5_update(context, padding, pad_len);
    md5_update(context, (char *)bits, 8);

    for(size_t i = 0, j = 0; j < 16; i++, j += 4) {
        hash[j] = (uint8_t)(context->state[i] & 0xFF);
        hash[j + 1] = (uint8_t)((context->state[i] >> 8) & 0xFF);
        hash[j + 2] = (uint8_t)((context->state[i] >> 16) & 0xFF);
        hash[j + 3] = (uint8_t)((context->state[i] >> 24) & 0xFF);
    }
    
    // Zeroize sensitive information
    char *data = (char *)context;
    for(size_t i = 0; i < sizeof(md5_context_t); i++) {
        data[i] = 0;
    }
}

void md5_to_string(uint8_t hash[16], char string[33]) {
    for(size_t i = 0; i < 16; i++) {
        sprintf(string + i * 2, "%02x", hash[i]);
    }
    string[32] = 0;
}

void generate_md5_hash(char *data, size_t data_size, char hash[33]) {
    md5_context_t context;
    context.count[0] = context.count[1] = 0;
    context.state[0] = 0x67452301;
    context.state[1] = 0xefcdab89;
    context.state[2] = 0x98badcfe;
    context.state[3] = 0x10325476;

    uint8_t hash_bytes[16];
    md5_update(&context, data, data_size);
    md5_final(hash_bytes, &context);
    md5_to_string(hash_bytes, hash);
}
