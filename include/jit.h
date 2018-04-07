#ifndef _JIT_H_
#define _JIT_H_

#include <stdint.h>

struct packet_pool_t;
typedef void (*benchmark_test_run_t)(struct packet_pool_t *, uint32_t);

enum jit_type {
    JIT_NONE = 0,
    JIT_TEST = 1, // Load one of the benchmarks in the tests/ folder
};

struct jit_t {
    void *handle;
    enum jit_type type;
    union {
        benchmark_test_run_t test;
    } entry;
};

void   jit_test_load(struct jit_t *, char const *name);
void   jit_test_unload(struct jit_t *);

#endif // _JIT_H_
