#ifndef _PACKETS_H_
#define _PACKETS_H_

#include "util.h"

struct packet_t {
    uint32_t size;              // size of the packet
    uint64_t arrival_time;      // arrival time for the packet
    uint64_t time;              // variable keeping track of time

    char *hdr;
    char *payload;
    void *metadata;

    __attribute__((aligned(64))) char data[];
};

typedef struct packet_t packet_t;
typedef int packet_index_t;


struct packet_pool_t {
    uint32_t count;
    uint32_t size;

    packet_t *cur;
    packet_t *packets;
    packet_t *end;

    packet_index_t (*pool_next_batch) (struct packet_pool_t *, packet_t **, packet_index_t);
    struct packet_t* (*pool_reset) (struct packet_pool_t *);

    void *metadata;
    char extra[256];
};

typedef struct packet_pool_t packet_pool_t;

packet_pool_t* packets_pool_create(uint32_t count, uint32_t size);
void           packets_pool_zipfian(packet_pool_t *, uint32_t from, 
                    uint32_t to, uint32_t offset, uint32_t bytes, double dist);

// Delete the packet pool
void                packets_pool_delete(packet_pool_t **);

// Return the next packet in the pool
packet_t*           packets_pool_next(packet_pool_t *);

// Get the next batch of packets of at most the passed size; return the number of packets in the batch.
packet_index_t      packets_pool_next_batch(packet_pool_t *, packet_t **, packet_index_t);

// Reset and return the first packet in the pool
packet_t*           packets_pool_reset(packet_pool_t *);

// Return the first packet in the packet pool
packet_t*           packets_pool_first(packet_pool_t *);

#endif // _PACKETS_H_
