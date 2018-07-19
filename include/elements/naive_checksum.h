#ifndef _ELEMENTS_NAIVE_CHECKSUM_H_
#define _ELEMENTS_NAIVE_CHECKSUM_H_

#include "element.h"
#include "rte_prefetch.h"

struct packet_t;

struct naive_checksum_t {
    struct element_t element;

    uint64_t naive_checksum_count;
};

struct naive_checksum_t *naive_checksum_create(void);
void naive_checksum_process(struct element_t *, struct packet_t **, packet_index_t);
void naive_checksum_release(struct element_t *);
void naive_checksum_report(struct element_t *);

#endif // _ELEMENTS_NAIVE_CHECKSUM_H_
