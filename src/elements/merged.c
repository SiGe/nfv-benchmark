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

#define MP_SIZE 32
#define MP_SIZE_HALF (MP_SIZE >> 1)

#define MP_TBL_SIZE (1<<24)


struct merged_t *merged_create(void) {
    struct merged_t *merged = (struct merged_t *)mem_alloc(sizeof(struct merged_t));
    memset(merged, 0, sizeof(struct merged_t));

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
    merged->tbl = mem_alloc(sizeof(uint32_t) * merged->tbl_size);
    memset(merged->tbl, 0, sizeof(uint32_t) * merged->tbl_size);

    // Setup checksum
    // nop;

    return merged;
}

void merged_process(struct element_t *ele, struct packet_t **pkts, packet_index_t size) {
    struct merged_t *self = (struct merged_t*)ele;
    // Use the two/two prefetching pattern
    struct packet_t *pkt;
    struct packet_t *p[MP_SIZE];
    uint32_t hashes[MP_SIZE_HALF];
    struct __attribute__((packed)) {
        ipv4_t src;
        ipv4_t dst;
        union {
            struct __attribute__((packed)) {
                uint16_t src;
                uint16_t dst;
            } ports;
            uint32_t srcdst_port;
        };
    } ip;

    uint32_t out;

    for (int j = 0; j < MP_SIZE_HALF; ++j) {
        p[j] = pkts[j];
        rte_prefetch0(p[j]->hdr + 26);
    }

    int i = MP_SIZE_HALF;
    for (;i < size - MP_SIZE_HALF; i += MP_SIZE_HALF) {
        for (int j = 0; j < MP_SIZE_HALF && i + j < size; ++j) {
            p[j + MP_SIZE_HALF] = pkts[i + j];
            // Prefetch the next set of packets
            rte_prefetch0(p[j + MP_SIZE_HALF]->hdr + 26);
        }

        for (int j = 0; j < MP_SIZE_HALF; ++j) {
            pkt = p[j];
            ip.src = *((ipv4_t*)(pkt->hdr+ 14 + 12));
            ip.dst = *((ipv4_t*)(pkt->hdr+ 14 + 12 + 4));
            ip.srcdst_port = *((uint32_t*)(pkt->hdr+ 14 + 12 + 8));

            out = util_hash_ret(&ip, sizeof(ip));
            out &= ((MP_TBL_SIZE) - 1);
            hashes[j] = out;
            rte_prefetch0(self->tbl + out);

            struct _routing_tbl_entry_t *ent = routing_entry_find(self, ip.dst);
            if (ent) {
                self->port_count += ent->port;
                ent->count ++;
            }

            self->checksum_count += checksum(pkt->hdr, pkt->size);
        }

        for (int j = 0; j < MP_SIZE_HALF; ++j) {
            self->tbl[hashes[j]]++;
            p[j] = p[j + MP_SIZE_HALF];
        }
    }

    i -= MP_SIZE_HALF;
    for (int j = i; j < size; ++j) {
        pkt = pkts[j];
        ip.src = *((ipv4_t*)(pkt->hdr+ 14 + 12));
        ip.dst = *((ipv4_t*)(pkt->hdr+ 14 + 12 + 4));
        ip.srcdst_port = *((uint32_t*)(pkt->hdr+ 14 + 12 + 8));

        out = util_hash_ret(&ip, sizeof(ip));
        out &= ((MP_TBL_SIZE) - 1);
        hashes[j - i] = out;
        rte_prefetch0(self->tbl + out);

        struct _routing_tbl_entry_t *ent = routing_entry_find(self, ip.dst);
        if (ent) {
            self->port_count += ent->port;
            ent->count ++;
        }

        self->checksum_count += checksum(pkt->hdr, pkt->size);
    }

    for (int j = i; j < size; ++j) {
        self->tbl[hashes[j - i]]++;
    }

    element_dispatch(ele, 0, pkts, size);
}

void merged_release(struct element_t *ele) {
    struct merged_t *self = (struct merged_t *)ele;

    uint64_t total = 0;
    for (int i = 0 ; i < self->tbl_size; ++i) {
        total += self->tbl[i];
    }
    printf("Total number of packets processed: %llu\n", total);

    if (self->tbl) {
        mem_release(self->tbl);
    }
    mem_release(ele);
}

void merged_report(__attribute__((unused)) struct element_t *_) {
    // VOID
}
