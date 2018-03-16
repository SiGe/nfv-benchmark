#include "benchmark.h"

void benchmark_config_init(struct benchmark_t *bench) {
    struct pipeline_t *pipe = 0;

    bench->repeat = REPEAT;
    bench->pool = packets_pool_create(1<<20, PACKET_SIZE);
    bench->pipeline = pipe = pipeline_create();

    pipeline_element_add(pipe, el_checksum_create(MOD_BUFFER_SIZE_1));
    pipeline_element_add(pipe, el_drop_create(MOD_BUFFER_SIZE_2));
}
