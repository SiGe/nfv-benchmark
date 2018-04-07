#include "log.h"
#include "packets.h"
#include "pipeline.h"
#include "benchmark.h"

#include <stdio.h>

void jit_test(struct packet_pool_t *pool, uint32_t repeat) {
    log_info("In the jit test module.");

    packet_index_t batch_size = 0;
    packet_t *pkts[32] = {0};
    struct benchmark_t bench;
    benchmark_config_init(&bench);
    struct pipeline_t *pipe = bench.pipeline;
    uint64_t count = 0;

    for (uint32_t i = 0; i < repeat; ++i) {
        while ((batch_size = packets_pool_next_batch(pool, pkts, 32)) != 0) {
            pipeline_process(pipe, pkts, batch_size);
            count+= batch_size;
        }
        packets_pool_reset(pool);
    }

    pipeline_release(pipe);
}
