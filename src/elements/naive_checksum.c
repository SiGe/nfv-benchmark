#include <stdio.h>
#include <assert.h>

#include "checksum.h"
#include "memory.h"
#include "rte_prefetch.h"

#include "elements/naive_checksum.h"

struct naive_checksum_t *naive_checksum_create(void) {
    struct naive_checksum_t *checksum = (struct naive_checksum_t *)mem_alloc(sizeof(struct naive_checksum_t));

    checksum->element.process = naive_checksum_process;
    checksum->element.release = naive_checksum_release;
    checksum->element.report = naive_checksum_report;
    checksum->element.connect = element_connect;
    checksum->element.disconnect = element_disconnect;
    checksum->element.hop_at = element_hop_at;

    return checksum;
}

void naive_checksum_process(struct element_t *ele, struct packet_t **pkts, packet_index_t size) {
    struct naive_checksum_t *self = ((struct naive_checksum_t*)ele);

    struct packet_t *pkt = pkts[0];
    self->naive_checksum_count += checksum(pkt->hdr, pkt->size);
    element_dispatch(ele, 0, pkts, 1);
}

void naive_checksum_release(struct element_t *ele) {
    mem_release(ele);
}

void naive_checksum_report(__attribute__((unused)) struct element_t *_) {}
