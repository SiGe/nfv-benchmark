#ifndef _TYPES_H_
#define _TYPES_H_

#include <stdint.h>

typedef uint32_t ipv4_t;
typedef uint32_t maskv4_t;

typedef struct {
    ipv4_t ipv4;
    maskv4_t mask;
} ipv4_prefix_t;

#endif
