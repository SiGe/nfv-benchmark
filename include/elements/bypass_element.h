#ifndef _BYPASS_H_
#define _BYPASS_H_

#include "element.h"
#include "memory.h"

struct bypass_element_t {
    // this definition should always be at the beginning
    struct element_t self;

    struct element_t *element;
    packet_index_t notify_size;
};

struct bypass_element_t *bypass_element_create(struct element_t *ele, packet_index_t notify_size);
void bypass_element_process(element_t *self, packet_t **pkts, packet_index_t size);
void bypass_element_release(element_t *self);
void bypass_element_report(element_t *self);
void bypass_element_connect(element_t *self, port_index_t, element_t*);
void bypass_element_disconnect(element_t *self, port_index_t);
element_t *bypass_element_hop_at(element_t *self, port_index_t);

static inline void bypass_element_add(struct bypass_element_t *buf, packet_t* pkt) {
    buf->buffer[buf->size++] = pkt;
}

#endif // _BYPASS_H_
