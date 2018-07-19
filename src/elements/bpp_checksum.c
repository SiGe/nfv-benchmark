#include <stdio.h>
#include <assert.h>

#include "checksum.h"
#include "memory.h"
#include "rte_prefetch.h"

#include "elements/bpp_checksum.h"

struct bpp_checksum_t *bpp_checksum_create(void) {
    struct bpp_checksum_t *checksum = (struct bpp_checksum_t *)mem_alloc(sizeof(struct bpp_checksum_t));

    checksum->element.process = bpp_checksum_process;
    checksum->element.release = bpp_checksum_release;
    checksum->element.report = bpp_checksum_report;
    checksum->element.connect = element_connect;
    checksum->element.disconnect = element_disconnect;
    checksum->element.hop_at = element_hop_at;

    return checksum;
}

void bpp_checksum_process(struct element_t *ele, struct packet_t **pkts, packet_index_t size) {
    struct bpp_checksum_t *self = ((struct bpp_checksum_t*)ele);

    for (packet_index_t i = 0; i < size; ++i) {
        rte_prefetch0(pkts[i]->hdr);
    }

    struct packet_t *pkt = 0;
    for (packet_index_t i = 0; i < size; ++i) {
        pkt = pkts[i];
        self->bpp_checksum_count += checksum(pkt->hdr, pkt->size);
    }

    element_dispatch(ele, 0, pkts, size);
}

void bpp_checksum_release(struct element_t *ele) {
    mem_release(ele);
}

void bpp_checksum_report(__attribute__((unused)) struct element_t *_) {}
