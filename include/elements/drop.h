#ifndef _ELEMENTS_DROP_H_
#define _ELEMENTS_DROP_H_

#include "element.h"

struct packet_t;

struct drop_t {
    struct element_t element;

    uint64_t drop_count;
};

struct drop_t *drop_create(void);
void drop_process(struct element_t *, struct packet_t **, packet_index_t);
void drop_release(struct element_t *);
void drop_report(struct element_t *);

#endif // _ELEMENTS_DROP_H_
