#ifndef _ELEMENTS_IDENTITY_H_
#define _ELEMENTS_IDENTITY_H_

#include "element.h"

struct packet_t;

struct identity_t {
    struct element_t element;

    uint64_t identity_count;
};

struct identity_t *identity_create(void);
void identity_process(struct element_t *, struct packet_t **, packet_index_t);
void identity_release(struct element_t *);
void identity_report(struct element_t *);

#endif // _ELEMENTS_IDENTITY_H_
