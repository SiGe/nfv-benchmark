#ifndef _ELEMENTS_DROP_MBUF_H_
#define _ELEMENTS_DROP_MBUF_H_

#include "element.h"
#include "dataplane.h"

struct packet_t;

struct drop_mbuf_t {
    struct element_t element;

    struct rx_packet_stream *rx_stream;
    uint64_t drop_count;
    uint32_t *hist;
};

struct drop_mbuf_t *drop_mbuf_create(struct rx_packet_stream *);
void drop_mbuf_process(struct element_t *, struct packet_t **, packet_index_t);
void drop_mbuf_release(struct element_t *);
void drop_mbuf_report(struct element_t *);

#endif // _ELEMENTS_DROP_H_
