#ifndef _ELEMENTS_BP_MEASUREMENT_H_
#define _ELEMENTS_BP_MEASUREMENT_H_

#include "element.h"

#undef MEASUREMENT_POPULAR

struct packet_t;

struct bp_measurement_t {
    struct element_t element;
    size_t tbl_size;
    uint32_t _tmp[MEASUREMENT_BUFFER_SIZE];

#ifdef MEASUREMENT_POPULAR
    uint32_t pop_count;
    uint32_t pop_sig[3];
#endif

    uint32_t *tbl;
};

struct bp_measurement_t *bp_measurement_create(void);
void bp_measurement_resize(struct bp_measurement_t *, size_t tbl_size);
void bp_measurement_process(struct element_t *, struct packet_t **, packet_index_t);
void bp_measurement_release(struct element_t *);
void bp_measurement_report(struct element_t *);

#endif // _ELEMENTS_BP_MEASUREMENT_H_
