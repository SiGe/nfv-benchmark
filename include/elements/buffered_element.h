#ifndef _BATCH_H_
#define _BATCH_H_

#include "element.h"
#include "memory.h"

struct buffered_element_t {
    // this definition should always be at the beginning
    struct element_t self;

    packet_index_t size;
    struct element_t *element;
    packet_index_t notify_size;

    struct packet_t *buffer[];
};

struct buffered_element_t *buffered_element_create(
        struct element_t *ele,
        packet_index_t notify_size);
void buffered_element_process(
        element_t *self, packet_t **pkts, packet_index_t size);
void buffered_element_release(element_t *self);
void buffered_element_report(element_t *self);
void buffered_element_connect(element_t *self, port_index_t, element_t*);
void buffered_element_disconnect(element_t *self, port_index_t);
element_t *buffered_element_hop_at(element_t *self, port_index_t);


static inline void buffered_element_add(
        struct buffered_element_t *buf, packet_t* pkt) {
    buf->buffer[buf->size++] = pkt;
}

#endif // _BATCH_H_
