#ifndef _ELEMENTS_TIMER_H_
#define _ELEMENTS_TIMER_H_

#include "element.h"

struct packet_t;

struct timer_t {
    struct element_t element;

    uint64_t timer_count;
};


struct timer_t *timer_packet_create(void);
void timer_resize(struct timer_t *, size_t tbl_size);
void timer_process(struct element_t *, struct packet_t **, packet_index_t);
void timer_release(struct element_t *);
void timer_report(struct element_t *);

#endif // _ELEMENTS_TIMER_H_
