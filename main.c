#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "benchmark.h"
#include "jit.h"
#include "log.h"
#include "packets.h"
#include "pipeline.h"

#include "rte_cycles.h"
#include "rte_prefetch.h"


void test_benchmark(char const *name) {
    uint32_t packet_count = 1<<24;
    struct packet_pool_t *pool = packets_pool_create(packet_count, PACKET_SIZE);

    // Create a zipfian distribution for source/destination ip address
    packets_pool_zipfian(pool, 0, packet_count - 1, 26, 8, 0.5);

    // Compile and load the checksum-drop module
    struct jit_t jit = {0};
    jit_test_load(&jit, name);

    // Benchmark the running time of the jitted test
    // Put a memory barrier for benchmarks
    uint32_t repeat = 20;
    asm volatile ("mfence" ::: "memory");
    uint64_t cycles = rte_get_tsc_cycles();
    (*jit.entry.test)(pool, repeat);
    asm volatile ("mfence" ::: "memory");
    printf("num cycles per packet (%.2f)\n", (float)(rte_get_tsc_cycles() - cycles)/(float)(packet_count * repeat));

    // Unload once done
    jit_test_unload(&jit);
    packets_pool_delete(&pool);
}

int main() {
    // Deterministic experiments are the best experiments - one can only hope.
    srand(0);
    test_benchmark("measurement-drop");
    return 0;
}
