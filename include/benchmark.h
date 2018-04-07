#ifndef _BENCHMARK_H_
#define _BENCHMARK_H_

#include <stdint.h>
#include "pipeline.h"

struct pipeline_t;
struct packet_pool_t;

struct benchmark_t {
    struct pipeline_t *pipeline;
    struct packet_pool_t *pool;
    uint32_t repeat;
};

typedef void (*benchmark_config_init_t) (struct benchmark_t *);
void benchmark_config_init(struct benchmark_t *);

#endif
