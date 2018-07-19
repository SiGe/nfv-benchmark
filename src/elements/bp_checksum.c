#include <stdio.h>
#include <assert.h>

#include "checksum.h"
#include "memory.h"
#include "rte_prefetch.h"

#include "elements/bp_checksum.h"

struct bp_checksum_t *bp_checksum_create(void) {
    struct bp_checksum_t *checksum = (struct bp_checksum_t *)mem_alloc(sizeof(struct bp_checksum_t));

    checksum->element.process = bp_checksum_process;
    checksum->element.release = bp_checksum_release;
    checksum->element.report = bp_checksum_report;
    checksum->element.connect = element_connect;
    checksum->element.disconnect = element_disconnect;
    checksum->element.hop_at = element_hop_at;

    return checksum;
}

void bp_checksum_process(struct element_t *ele, struct packet_t **pkts, packet_index_t size) {
    struct bp_checksum_t *self = ((struct bp_checksum_t*)ele);

    struct packet_t *pkt = 0;
    for (packet_index_t i = 0; i < size; ++i) {
        pkt = pkts[i];
        self->bp_checksum_count += checksum(pkt->hdr, pkt->size);
    }

    element_dispatch(ele, 0, pkts, size);
}

void bp_checksum_release(struct element_t *ele) {
    mem_release(ele);
}

void bp_checksum_report(__attribute__((unused)) struct element_t *_) {}
