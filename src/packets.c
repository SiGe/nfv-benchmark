#include <stdio.h>

#include "defaults.h"
#include "memory.h"
#include "rte_prefetch.h"
#include "util.h"

#include "packets.h"

// Use pool->size instead of pkt->size to avoid needing to read the packet
#define packet_end(pool, pkt) (pkt->data + pool->size)

inline packet_t *packets_pool_first(packet_pool_t *pool) {
    return pool->packets;
}

inline packet_t *packets_pool_next(packet_pool_t *pool) {
    pool->cur = (packet_t*)packet_end(pool, pool->cur);
    return (pool->cur < pool->end) ? pool->cur: 0;
}

inline packet_index_t packets_pool_next_batch(
        packet_pool_t *pool, packet_t **pkts, packet_index_t pool_size) {
    for (packet_index_t i = 0; i < pool_size; ++i) {
        if (pool->cur >= pool->end) {
            return i;
        }

        // Prefetch the packet for processing
        rte_prefetch0(pool->cur);
        pkts[i] = pool->cur;
        pool->cur = (packet_t *)packet_end(pool, pool->cur);
    }

    return pool_size;
}

inline packet_t *packets_pool_reset(packet_pool_t *pool) {
    pool->cur = pool->packets;
    return pool->cur;
}

packet_pool_t* packets_pool_create(uint32_t count, uint32_t size) {
    size_t ds_size = ALIGN(sizeof(packet_t) + size, 6);
    //size_t ds_size = sizeof(packet_t) + size;
    printf("SIZE OF PACKETS: %lu\n", ds_size);
    size_t pool_size = ds_size * count;
    void *mem = mem_alloc_align(CACHE_LINE_SIZE, pool_size);
    packet_pool_t *pool = mem_alloc_align(CACHE_LINE_SIZE, sizeof(packet_pool_t));

    pool->packets = mem;
    pool->count = count;
    pool->size = ds_size - (sizeof(packet_t));
    pool->end = mem + pool_size;
    pool->cur = mem;

    for (packet_t *pkt = packets_pool_first(pool); 
            pkt < pool->end; pkt = (packet_t*)(pkt->data + pool->size)) {
        pkt->size = size;
        pkt->hdr = pkt->data;

        // Feel packet headers with junk data.
        uint64_t *junk = 0;
        for (int i = 0; i < 8; ++i) {
            junk = (uint64_t*)(pkt->hdr + i*8);
            *junk = rand64_t();
        }
        pkt->payload = pkt->hdr + 64;
    }

    return pool;
}

void packets_pool_delete(packet_pool_t **pool) {
    free(*pool);
    *pool = 0;
}
