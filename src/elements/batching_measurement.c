#include "log.h"
#include "memory.h"
#include "murmur3.h"
#include "packets.h"
#include "rte_prefetch.h"
#include "util.h"
#include "types.h"

#include "elements/batching_measurement.h"

void batching_measurement_resize(struct batching_measurement_t *self, size_t size) {
    size = ge_pow2_64(size);
    self->tbl_size = size;
    self->tbl = mem_realloc(self->tbl, sizeof(uint32_t) * size);
}

struct batching_measurement_t *batching_measurement_create(void) {
    struct batching_measurement_t *measurement = (struct batching_measurement_t *)mem_alloc(sizeof(struct batching_measurement_t));

    measurement->element.process = batching_measurement_process;
    measurement->element.release = batching_measurement_release;
    measurement->element.report = batching_measurement_report;
    measurement->element.connect = element_connect;
    measurement->element.disconnect = element_disconnect;
    measurement->element.hop_at = element_hop_at;
    measurement->tbl_size = 0;
    measurement->tbl = 0;

    return measurement;
}

void batching_measurement_process(struct element_t *ele, struct packet_t **pkts, packet_index_t size) {
    struct batching_measurement_t *self = (struct batching_measurement_t *)ele;
    struct __attribute__((packed)) {
        ipv4_t src;
        ipv4_t dst;
        uint16_t src_port;
        uint16_t dst_port;
    } ip;

    uint32_t out;
    size_t size_minus_one = self->tbl_size - 1;

    struct packet_t *pkt = 0;
    for (packet_index_t i = 0; i < size; ++i) {
        char *hdr = pkts[i]->hdr;
        ip.src = *((ipv4_t*)(hdr + 14 + 12));
        ip.dst = *((ipv4_t*)(hdr + 14 + 12 + 4));
        ip.src_port = *((uint16_t*)(hdr + 14 + 20 + 0));
        ip.dst_port = *((uint16_t*)(hdr + 14 + 20 + 2));

        //util_hash(&ip, sizeof(ip), &out);
        out = util_hash_ret(&ip, sizeof(ip));

        // Make sure that the tbl_size is a multiple of two
        self->tbl[out & size_minus_one]++;
    }

    element_dispatch(ele, 0, pkts, size);
}

void batching_measurement_release(struct element_t *ele) {
    struct batching_measurement_t *self = (struct batching_measurement_t *)ele;
    if (self->tbl) {
        printf("Printing measurement moduel stats\n");
        size_t size_minus_one = self->tbl_size - 1;
        size_t total_unique = 0;
        size_t total_count = 0;
        for (int i = 0; i < size_minus_one; ++i) {
            total_count += self->tbl[i];
            total_unique += (self->tbl[i] != 0);
        }
        printf("Total unique: %d, total count: %d\n", total_unique, total_count);
        mem_release(self->tbl);
    }
    mem_release(self);
}

void batching_measurement_report(__attribute__((unused)) struct element_t *_) {
    // VOID
}
