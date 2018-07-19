#ifndef _ELEMENTS_NAIVE_MEASUREMENT_H_
#define _ELEMENTS_NAIVE_MEASUREMENT_H_

#include "element.h"

#undef MEASUREMENT_POPULAR

struct packet_t;

struct naive_measurement_t {
    struct element_t element;
    size_t tbl_size;
    uint32_t _tmp[MEASUREMENT_BUFFER_SIZE];

#ifdef MEASUREMENT_POPULAR
    uint32_t pop_count;
    uint32_t pop_sig[3];
#endif

    uint32_t *tbl;
};

struct naive_measurement_t *naive_measurement_create(void);
void naive_measurement_resize(struct naive_measurement_t *, size_t tbl_size);
void naive_measurement_process(struct element_t *, struct packet_t **, packet_index_t);
void naive_measurement_release(struct element_t *);
void naive_measurement_report(struct element_t *);

#endif // _ELEMENTS_NAIVE_MEASUREMENT_H_
