#include <assert.h>
#include <stdio.h>

#include "defaults.h"
#include "log.h"
#include "memory.h"
#include "rte_prefetch.h"
#include "util.h"
#include "zipf.h"

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

static packet_t *packets_pool_at(packet_pool_t *pool, packet_index_t idx) {
    return pool->packets + (sizeof(struct packet_t) + pool->size) * idx;
}

static packet_t *packets_pool_move(packet_pool_t *pool, packet_index_t idx) {
    pool->cur = packets_pool_at(pool, idx);
    return pool->cur;
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

void packets_pool_zipfian(packet_pool_t *pool, uint32_t from, 
        uint32_t to, uint32_t offset, uint32_t bytes, double dist) {
    assert(offset >= 0 && (offset + bytes < pool->size));
    assert(from >= 0 && to < pool->count && from < to);
    packet_t *end = packets_pool_at(pool, to-1);
    rand_val(2000);

    int num_values = (to - from);

    // TODO: just use num_values as the total number of unique values - there
    // should be a way to automatically calculate this though
    int n = num_values; 
    zipf(dist, n, 1);

    log_info("Regenerated Zipf distribution.");
    // Fill the bytestream with random values
    char *bytestream = mem_alloc(bytes * num_values);
    int *iptr = (int *)bytestream;
    for (int i = 0; i < bytes/(sizeof(int)); ++i) { // Eh, ignore the last byte
        *iptr++ = rand();
    }

    int count = 0;
    for (struct packet_t *pkt = packets_pool_move(pool, from);
            pkt && (pkt != end); pkt = packets_pool_next(pool)) {
        int index = zipf(dist, n, 0);
        memcpy(pkt->data + offset, bytestream + (bytes * (index-1)), bytes);
        count ++;
    }
    assert(count == (to - from + 1));
    packets_pool_reset(pool);
    mem_release(bytestream);
    log_info("Completed generating a Zipfian traffic pattern.");
}

void packets_pool_delete(packet_pool_t **pool) {
    free(*pool);
    *pool = 0;
}
