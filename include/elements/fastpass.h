#ifndef _FASTPASS_H_
#define _FASTPASS_H_

#include "element.h"
#include "memory.h"

#define MAX_FASTPASS_PACKETS 256

struct fastpass_t {
    // this definition should always be at the beginning
    struct element_t element;
    struct packet_t *fast[MAX_FASTPASS_PACKETS];
    struct packet_t *slow[MAX_FASTPASS_PACKETS];

    // 4-tuples of the flow
    uint32_t sig[3];

    // Measurement
    uint32_t count;

    // Routing
    uint32_t port;

    // Checksum
    uint32_t checksum_count;
};

struct fastpass_t *fastpass_create(
        uint32_t a, uint32_t b, uint32_t c);

void fastpass_process(
        element_t *self, packet_t **pkts, packet_index_t size);

void fastpass_release(element_t *self);
void fastpass_report(element_t *self);
void fastpass_connect(element_t *self, port_index_t, element_t*);
void fastpass_disconnect(element_t *self, port_index_t);
element_t *fastpass_hop_at(element_t *self, port_index_t);

#endif // _FASTPASS_H_
