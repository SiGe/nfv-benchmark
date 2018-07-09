#include "checksum.h"
#include "memory.h"
#include "rte_prefetch.h"
#include "element.h"

#include "elements/fastpass.h"

#define PREFETCH_SIZE 16
#define PREFETCH_SIZE_HALF (PREFETCH_SIZE >> 1)

struct fastpass_t *fastpass_create(uint32_t a, uint32_t b, uint32_t c) {
    struct fastpass_t *fastpass = (struct fastpass_t *)mem_alloc(sizeof(struct fastpass_t));

    fastpass->element.process = fastpass_process;
    fastpass->element.release = fastpass_release;
    fastpass->element.report = fastpass_report;
    fastpass->element.connect = element_connect;
    fastpass->element.disconnect = element_disconnect;
    fastpass->element.hop_at = element_hop_at;

    fastpass->count = 0;
    fastpass->port = 0;
    fastpass->sig[0] = a;
    fastpass->sig[1] = b;
    fastpass->sig[2] = c;

    return fastpass;
}

void fastpass_process(struct element_t *ele, struct packet_t **pkts, packet_index_t size) {
    struct fastpass_t *self = (struct fastpass_t *)ele;

    // for (int i = 0; i < size; ++i) {
    //     rte_prefetch0(pkts[i]);
    // }

    // for (int i = 0; i < size; ++i) {
    //     rte_prefetch0(pkts[i]->hdr);
    // }

    uint32_t fast = 0;
    uint32_t slow = 0;
    uint32_t a, b, c;
    a = self->sig[0];
    b = self->sig[1];
    c = self->sig[2];

    // Use the two/two prefetching pattern
    struct packet_t *p[PREFETCH_SIZE];
    for (int j = 0; j < PREFETCH_SIZE_HALF; ++j) {
        p[j] = pkts[j];
    }

    int i = PREFETCH_SIZE_HALF;
    for (; i < size - PREFETCH_SIZE_HALF; i += PREFETCH_SIZE_HALF) {
        for (int j = 0; j < PREFETCH_SIZE_HALF && i + j < size; ++j) {
            p[j+PREFETCH_SIZE_HALF] = pkts[i+j];
            rte_prefetch0(p[j+PREFETCH_SIZE_HALF]->hdr + 26);
        }

        for (int j = 0; j < PREFETCH_SIZE_HALF; ++j) {
            self->fast[fast++] = self->slow[slow++] = p[j];
            uint32_t is_fast = (*((uint32_t*)(p[j]->hdr + 14 + 16)) == b) &&
                               (*((uint32_t*)(p[j]->hdr + 14 + 12)) == a) &&
                               (*((uint32_t*)(p[j]->hdr + 14 + 20)) == c);

            self->count += is_fast;
            self->port  += (is_fast & self->port);
            fast -= (1 - is_fast);
            slow -= is_fast;
        }

        for (int j = 0; j < PREFETCH_SIZE_HALF; ++j) {
            p[j] = p[j + PREFETCH_SIZE_HALF];
        }
    }

    i -= PREFETCH_SIZE_HALF;
    for (int j = i; j < size; ++j) {
        self->fast[fast++] = self->slow[slow++] = pkts[j];
        uint32_t is_fast = (*((uint32_t*)(pkts[j]->hdr + 14 + 16)) == b) &&
                           (*((uint32_t*)(pkts[j]->hdr + 14 + 20)) == c) &&
                           (*((uint32_t*)(pkts[j]->hdr + 14 + 12)) == a);

        self->count += is_fast;
        self->port  += (is_fast & self->port);
        fast -= (1 - is_fast);
        slow -= is_fast;
    }

    for (packet_index_t i = 0; i < fast; ++i) {
        packet_t *pkt = self->fast[i];
        self->checksum_count += checksum(pkt->hdr, pkt->size);
    }

    element_dispatch(ele, 1, self->fast, fast);
    element_dispatch(ele, 0, self->slow, slow);
}

void fastpass_release(struct element_t *ele) {
    printf("Fast: %d\n", ((struct fastpass_t *)ele)->count);
    mem_release(ele);
}

void fastpass_report(__attribute__((unused)) struct element_t *_) {
    // VOID
}


