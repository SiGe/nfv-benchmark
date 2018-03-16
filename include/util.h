#ifndef _UTIL_H_
#define _UTIL_H_

#include <stdint.h>
#include <stdlib.h>
#include "murmur3.h"

#define UTIL_SEED 43975688

#define util_hash(key, len, out) MurmurHash3_x64_128(key, len, UTIL_SEED, out)

#define ALIGN(size, to) ((size + ((1<<to) - 1)) & (-1 ^ ((1<<to) - 1)));

static inline uint64_t rand64_t() {
    return ((uint64_t)rand() << 32) | ((uint64_t)rand());
}

#endif // _UTIL_H_
