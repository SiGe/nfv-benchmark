#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "rte_ethdev.h"
#include "rte_malloc.h"

#include "benchmark.h"
#include "benchmark_helper.h"
#include "dataplane.h"
#include "pipeline.h"

struct pipeline_t *pipeline_build(struct rx_packet_stream *stream) {
    struct pipeline_t *pipe = pipeline_create();
    pipeline_element_add(pipe, el_timer_create(MOD_BUFFER_SIZE_1));
    pipeline_element_add(pipe, el_checksum_create(MOD_BUFFER_SIZE_2));
    pipeline_element_add(pipe, el_checksum_create(MOD_BUFFER_SIZE_3));
    pipeline_element_add(pipe, el_drop_mbuf_create(MOD_BUFFER_SIZE_DROP, stream));
    return pipe;
}
