#ifndef _ELEMENTS_CHECKSUM_H_
#define _ELEMENTS_CHECKSUM_H_

#include "element.h"

struct packet_t;

struct checksum_t {
    struct element_t element;

    uint64_t checksum_count;
};

struct checksum_t *checksum_create();
void checksum_process(struct element_t *, struct packet_t **, packet_index_t);
void checksum_release(struct element_t *);
void checksum_report(struct element_t *);

#endif // _ELEMENTS_CHECKSUM_H_
