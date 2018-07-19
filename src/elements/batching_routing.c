#include <assert.h>
#include <stdio.h>

#include "defaults.h"
#include "element.h"
#include "memory.h"
#include "packets.h"
#include "rte_prefetch.h"
#include "types.h"

#include "elements/routing.h"
#include "elements/batching_routing.h"

#define PORT_MASK ((1<<ELEMENT_MAX_PORT_COUNT_LOG) - 1)

struct batching_routing_t *batching_routing_create(void) {
    struct batching_routing_t *ret = mem_alloc(sizeof(struct batching_routing_t));
    memset(ret, 0, sizeof(struct batching_routing_t));
    ret->element.process = batching_routing_process;
    ret->element.release = batching_routing_release;
    ret->element.report = batching_routing_report;
    ret->element.connect = element_connect;
    ret->element.disconnect = element_disconnect;
    ret->element.hop_at = element_hop_at;

    return ret;
}

void batching_routing_process(struct element_t *el, struct packet_t **pkts, packet_index_t size) {
    struct batching_routing_t *self = (struct batching_routing_t *)el;
    uint32_t port_count = 0;

    struct packet_t *pkt = 0;
    for (packet_index_t i = 0; i < size; ++i) {
        pkt = pkts[i];
        struct _routing_tbl_entry_t *ent = routing_entry_find(self, *((ipv4_t*)(pkt->hdr+ 14 + 12 + 4)));
        if (ent) {
            port_count += ent->port;
            ent->count ++;
        }
    }

    element_dispatch(el, port_count, pkts, size);
}

void batching_routing_release(struct element_t *el) {
    struct _routing_tbl_256_t *fst = &((struct batching_routing_t *)el)->fst_tbl;
    for (int i = 0; i < 256; ++i) {
        if (fst->entry[i].tbl_valid) {
            routing_release_tbl(fst->entry[i].tbl);
        }
    }
    mem_release(el);
}

void batching_routing_report(__attribute__((unused)) struct element_t *_) {
}
