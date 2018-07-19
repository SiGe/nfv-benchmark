#include <stdio.h>
#include <assert.h>

#include "checksum.h"
#include "memory.h"
#include "rte_prefetch.h"

#include "elements/batching_checksum.h"

struct batching_checksum_t *batching_checksum_create(void) {
    struct batching_checksum_t *checksum = (struct batching_checksum_t *)mem_alloc(sizeof(struct batching_checksum_t));

    checksum->element.process = batching_checksum_process;
    checksum->element.release = batching_checksum_release;
    checksum->element.report = batching_checksum_report;
    checksum->element.connect = element_connect;
    checksum->element.disconnect = element_disconnect;
    checksum->element.hop_at = element_hop_at;

    return checksum;
}

void batching_checksum_process(struct element_t *ele, struct packet_t **pkts, packet_index_t size) {
    struct batching_checksum_t *self = ((struct batching_checksum_t*)ele);

    struct packet_t *pkt = 0;
    for (packet_index_t i = 0; i < size; ++i) {
        pkt = pkts[i];
        self->batching_checksum_count += checksum(pkt->hdr, pkt->size);
    }

    element_dispatch(ele, 0, pkts, size);
}

void batching_checksum_release(struct element_t *ele) {
    mem_release(ele);
}

void batching_checksum_report(__attribute__((unused)) struct element_t *_) {}
