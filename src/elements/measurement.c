#include "memory.h"
#include "murmur3.h"
#include "packets.h"
#include "rte_prefetch.h"
#include "util.h"
#include "types.h"

#include "elements/measurement.h"

void measurement_resize(struct measurement_t *self, size_t size) {
    size = ge_pow2_64(size);
    self->tbl_size = size;
    self->tbl = mem_realloc(self->tbl, sizeof(uint32_t) * size);
}

struct measurement_t *measurement_create() {
    struct measurement_t *measurement = (struct measurement_t *)mem_alloc(sizeof(struct measurement_t));

    measurement->element.process = measurement_process_prefetching;
    measurement->element.release = measurement_release;
    measurement->element.report = measurement_report;
    measurement->element.connect = element_connect;
    measurement->element.disconnect = element_disconnect;
    measurement->element.hop_at = element_hop_at;
    measurement->tbl_size = 0;
    measurement->tbl = 0;

    return measurement;
}

void measurement_process_prefetching(struct element_t *ele, struct packet_t **pkts, packet_index_t size) {
    struct measurement_t *self = (struct measurement_t *)ele;
    struct __attribute__((packed)) {
        ipv4_t src;
        ipv4_t dst;
        uint16_t src_port;
        uint16_t dst_port;
    } ip;

    packet_index_t idx = size;
    uint32_t out;
    size_t size_minus_one = self->tbl_size - 1;
    struct packet_t **pkt_ptr = pkts;


    while (idx > MEASUREMENT_BUFFER_SIZE) {
        for (packet_index_t i = 0; i < MEASUREMENT_BUFFER_SIZE; ++i) {
            rte_prefetch0(pkt_ptr[i]->data + 26);
        }

        for (packet_index_t i = 0; i < MEASUREMENT_BUFFER_SIZE; ++i) {
            ip.src = *((ipv4_t*)(pkt_ptr[i]->data+ 14 + 12));
            ip.dst = *((ipv4_t*)(pkt_ptr[i]->data+ 14 + 12 + 4));
            ip.src_port = *((uint16_t*)(pkt_ptr[i]->data+ 14 + 20 + 0));
            ip.dst_port = *((uint16_t*)(pkt_ptr[i]->data+ 14 + 20 + 2));

            util_hash(&ip, sizeof(ip), &out);
            out &= size_minus_one;
            rte_prefetch0(self->tbl + out);
            self->_tmp[i] = out;
        }

        for (packet_index_t i = 0; i < MEASUREMENT_BUFFER_SIZE; ++i) {
            // Make sure that the tbl_size is a multiple of two
            self->tbl[self->_tmp[i]]++;
        }

        idx -= MEASUREMENT_BUFFER_SIZE;
        pkt_ptr += MEASUREMENT_BUFFER_SIZE;
    }

    if (idx > 0) {
        for (packet_index_t i = 0; i < idx; ++i) {
            rte_prefetch0(pkt_ptr[i]->data + 26);
        }

        for (packet_index_t i = 0; i < idx; ++i) {
            ip.src = *((ipv4_t*)(pkt_ptr[i]->data+ 14 + 12));
            ip.dst = *((ipv4_t*)(pkt_ptr[i]->data+ 14 + 12 + 4));
            ip.src_port = *((uint16_t*)(pkt_ptr[i]->data+ 14 + 20 + 0));
            ip.dst_port = *((uint16_t*)(pkt_ptr[i]->data+ 14 + 20 + 2));

            util_hash(&ip, sizeof(ip), &out);
            out &= size_minus_one;
            rte_prefetch0(self->tbl + out);
            self->_tmp[i] = out;
        }

        for (packet_index_t i = 0; i < idx; ++i) {
            // Make sure that the tbl_idx is a multiple of two
            self->tbl[self->_tmp[i]]++;
        }
    }

    element_dispatch(ele, 0, pkts, size);
}


void measurement_process_no_prefetching(struct element_t *ele, struct packet_t **pkts, packet_index_t size) {
    struct measurement_t *self = (struct measurement_t *)ele;
    struct __attribute__((packed)) {
        ipv4_t src;
        ipv4_t dst;
        uint16_t src_port;
        uint16_t dst_port;
    } ip;

    uint32_t out;
    size_t size_minus_one = self->tbl_size - 1;

    for (packet_index_t i = 0; i < size; ++i) {
        ip.src = *((ipv4_t*)(pkts[i]->hdr+ 14 + 12));
        ip.dst = *((ipv4_t*)(pkts[i]->hdr+ 14 + 12 + 4));
        ip.src_port = *((uint16_t*)(pkts[i]->hdr+ 14 + 20 + 0));
        ip.dst_port = *((uint16_t*)(pkts[i]->hdr+ 14 + 20 + 2));

        util_hash(&ip, sizeof(ip), &out);

        // Make sure that the tbl_size is a multiple of two
        self->tbl[out & size_minus_one]++;
    }

    element_dispatch(ele, 0, pkts, size);
}

void measurement_release(struct element_t *ele) {
    struct measurement_t *self = (struct measurement_t *)ele;
    if (self->tbl) {
        mem_release(self->tbl);
    }
    mem_release(self);
}

void measurement_report(struct element_t *_) {
    // VOID
}
