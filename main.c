#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "packets.h"

#include "rte_cycles.h"
#include "rte_prefetch.h"

#include "benchmark.h"
#include "pipeline.h"

void test_benchmark() {
    struct benchmark_t bench;
    benchmark_config_init(&bench);

    struct packet_pool_t *pool = bench.pool;
    struct pipeline_t *pipe = bench.pipeline;

    packet_index_t batch_size = 0;
    packet_t *pkts[32] = {0};
    uint64_t count = 0;
    
    // Put a memory barrier for benchmarks
    asm volatile ("mfence" ::: "memory");
    uint64_t cycles = rte_get_tsc_cycles();

    for (uint32_t i = 0; i < bench.repeat; ++i) {
        while (batch_size = packets_pool_next_batch(pool, pkts, 32)) {
            pipeline_process(pipe, pkts, batch_size);
            count+= batch_size;
        }
        packets_pool_reset(pool);
    }

    asm volatile ("mfence" ::: "memory");
    printf("num cycles per packet (%.2f)\n", (float)(rte_get_tsc_cycles() - cycles)/(float)(count));
    packets_pool_delete(&pool);
    pipeline_release(pipe);
}

int main() {
    // Deterministic experiments are the best experiments - one can only hope.
    srand(0);

    test_benchmark();
    return 0;
}
