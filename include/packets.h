#ifndef _PACKETS_H_
#define _PACKETS_H_

#include "util.h"

struct packet_t {
    uint32_t size;

    char *hdr;
    char *payload;

    __attribute__((aligned(64))) char data[];
};

typedef struct packet_t packet_t;

struct packet_pool_t {
    uint32_t count;
    uint32_t size;

    packet_t *cur;
    packet_t *packets;
    packet_t *end;
};

typedef struct packet_pool_t packet_pool_t;
typedef int packet_index_t;


packet_pool_t* packets_pool_create(uint32_t count, uint32_t size);
void           packets_pool_zipfian(packet_pool_t *, uint32_t from, 
                    uint32_t to, uint32_t offset, uint32_t bytes, double dist);

void                packets_pool_delete(packet_pool_t **);
packet_t*           packets_pool_next(packet_pool_t *);
packet_index_t      packets_pool_next_batch(packet_pool_t *, packet_t **, packet_index_t);
packet_t*           packets_pool_reset(packet_pool_t *);
packet_t*           packets_pool_first(packet_pool_t *);

#endif // _PACKETS_H_
