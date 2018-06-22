#include <stdio.h>
#include <assert.h>

#include "checksum.h"
#include "memory.h"
#include "rte_prefetch.h"

#include "elements/checksum.h"

struct checksum_t *checksum_create(void) {
    struct checksum_t *checksum = (struct checksum_t *)mem_alloc(sizeof(struct checksum_t));

    checksum->element.process = checksum_process;
    checksum->element.release = checksum_release;
    checksum->element.report = checksum_report;
    checksum->element.connect = element_connect;
    checksum->element.disconnect = element_disconnect;
    checksum->element.hop_at = element_hop_at;

    return checksum;
}

void checksum_process(struct element_t *ele, struct packet_t **pkts, packet_index_t size) {
    struct checksum_t *self = ((struct checksum_t*)ele);

    ELEMENT_TIME_START(pkts, size);

    for (packet_index_t i = 0; i < size; ++i) {
        packet_t *pkt = pkts[i];
        rte_prefetch0(pkt->hdr);
        //self->checksum_count += checksum(pkt->hdr, pkt->size);
    }

    for (packet_index_t i = 0; i < size; ++i) {
        packet_t *pkt = pkts[i];
        self->checksum_count += checksum(pkt->hdr, pkt->size);
    }

    ELEMENT_TIME_END(pkts, size);

    element_dispatch(ele, 0, pkts, size);
}

void checksum_release(struct element_t *ele) {
    mem_release(ele);
}

void checksum_report(__attribute__((unused)) struct element_t *_) {}
