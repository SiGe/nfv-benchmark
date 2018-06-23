#include "checksum.h"
#include "element.h"
#include "log.h"
#include "memory.h"
#include "murmur3.h"
#include "packets.h"
#include "rte_prefetch.h"
#include "types.h"
#include "util.h"

#include "elements/routing.h"
#include "elements/measurement.h"

#include "elements/merged.h"

#define MP_SIZE 16
#define MP_SIZE_HALF (PREFETCH_SIZE >> 1)

#define MP_TBL_SIZE (1<<24)


struct merged_t *merged_create(void) {
    struct merged_t *merged = (struct merged_t *)mem_alloc(sizeof(struct merged_t));
    memset(merged, 0, sizeof(struct(merged_t)));

    merged->element.process = merged_process;
    merged->element.release = merged_release;
    merged->element.report = merged_report;
    merged->element.connect = element_connect;
    merged->element.disconnect = element_disconnect;
    merged->element.hop_at = element_hop_at;

    // Setup routing pool
    // nop;

    // Setup measurement pool
    merged->tbl_size = ge_pow2_64(MP_TBL_SIZE);
    merged->tbl = mem_alloc(self->tbl, sizeof(uint32_t) * merged->tbl_size);

    // Setup checksum
    // nop;

    return merged;
}

void merged_process(struct element_t *ele, struct packet_t **pkts, packet_index_t size) {
    // Use the two/two prefetching pattern
    struct packet_t *p[MP_SIZE];
    uint32_t hashes[MP_SIZE_HALF];
    struct __attribute__((packed)) {
        ipv4_t src;
        ipv4_t dst;
        uint16_t src_port;
        uint16_t dst_port;
    } ip;

    for (int j = 0; j < MP_SIZE_HALF; ++j) {
        p[j] = pkts[j];
    }

    uint32_t remaining = size;

    for (int i = MP_SIZE_HALF; i < size; i += MP_SIZE_HALF) {
        remaining -= MP_SIZE_HALF;
        for (int j = 0; j < MP_SIZE_HALF; ++j) {
            p[j + MP_SIZE_HALF] = pkts[i + j];
            // Prefetch the next set of packets
            rte_prefetch0(p[j + MP_SIZE_HALF]->hdr + 26);
        }

        for (int j = 0; j < PREFETCH_SIZE_HALF; ++j) {
            ip.src = *((ipv4_t*)(pkts[i]->hdr+ 14 + 12));
            ip.dst = *((ipv4_t*)(pkts[i]->hdr+ 14 + 12 + 4));
            ip.src_port = *((uint16_t*)(pkts[i]->hdr+ 14 + 20 + 0));
            ip.dst_port = *((uint16_t*)(pkts[i]->hdr+ 14 + 20 + 2));

            util_hash(&ip, sizeof(ip), &out);
            out &= ((MP_TBL_SIZE) - 1);
            rte_prefetch0(self->tbl + out);
            hashes[j] = out;

            struct _routing_tbl_entry_t *ent = routing_entry_find(self, ip.dst);
            if (ent) {
                port_count += ent->port;
                ent->count ++;
            }

            self->checksum_count += checksum(pkt->hdr, pkt->size);
        }

        for (int j = 0; j < PREFETCH_SIZE_HALF; ++j) {
            self->tbl[hashes[j]]++;
            p[j] = p[j + PREFETCH_SIZE_HALF];
        }
    }

    for (int j = 0; j < remaining; ++j) {
        ip.src = *((ipv4_t*)(pkts[i]->hdr+ 14 + 12));
        ip.dst = *((ipv4_t*)(pkts[i]->hdr+ 14 + 12 + 4));
        ip.src_port = *((uint16_t*)(pkts[i]->hdr+ 14 + 20 + 0));
        ip.dst_port = *((uint16_t*)(pkts[i]->hdr+ 14 + 20 + 2));

        util_hash(&ip, sizeof(ip), &out);
        out &= ((MP_TBL_SIZE) - 1);
        rte_prefetch0(self->tbl + out);
        hashes[j] = out;

        struct _routing_tbl_entry_t *ent = routing_entry_find(self, ip.dst);
        if (ent) {
            port_count += ent->port;
            ent->count ++;
        }

        self->checksum_count += checksum(pkt->hdr, pkt->size);
    }

    for (int j = 0; j < remaining; ++j) {
        self->tbl[hashes[j]]++;
    }

    element_dispatch(ele, 0, pkts, size);
}

void merged_release(struct element_t *ele) {
    struct merged_t *self = (struct merged_t *)ele;
    if (self->tbl) {
        mem_release(self->tbl);
    }

    mem_release(ele);
}

void merged_report(__attribute__((unused)) struct element_t *_) {
    // VOID
}
