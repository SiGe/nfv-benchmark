#ifndef _UTIL_H_
#define _UTIL_H_

#include <stdint.h>
#include <stdlib.h>
#include "murmur3.h"

#define UTIL_SEED 43975688

#define util_hash(key, len, out) MurmurHash3_x86_32(key, len, UTIL_SEED, out)

#define ALIGN(size, to) ((size + ((1<<to) - 1)) & (-1 ^ ((1<<to) - 1)));

static inline uint64_t rand64_t(void) {
    return ((uint64_t)rand() << 32) | ((uint64_t)rand());
}

static inline uint32_t ge_pow2_32(uint32_t n) {
    n = n - 1;
    n = n | (n >> 1);
    n = n | (n >> 2);
    n = n | (n >> 4);
    n = n | (n >> 8);
    n = n | (n >> 16);
    return n + 1;
}

static inline uint64_t ge_pow2_64(uint64_t n) {
    n = n - 1;
    n = n | (n >> 1);
    n = n | (n >> 2);
    n = n | (n >> 4);
    n = n | (n >> 8);
    n = n | (n >> 16);
    n = n | (n >> 32);
    return n + 1;
}

#endif // _UTIL_H_
