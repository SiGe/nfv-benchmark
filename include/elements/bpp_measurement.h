#ifndef _ELEMENTS_BPP_MEASUREMENT_H_
#define _ELEMENTS_BPP_MEASUREMENT_H_

#include "element.h"

#undef MEASUREMENT_POPULAR

struct packet_t;

struct bpp_measurement_t {
    struct element_t element;
    size_t tbl_size;
    uint32_t _tmp[MEASUREMENT_BUFFER_SIZE];

#ifdef MEASUREMENT_POPULAR
    uint32_t pop_count;
    uint32_t pop_sig[3];
#endif

    uint32_t *tbl;
};

struct bpp_measurement_t *bpp_measurement_create(void);
void bpp_measurement_resize(struct bpp_measurement_t *, size_t tbl_size);
void bpp_measurement_process(struct element_t *, struct packet_t **, packet_index_t);
void bpp_measurement_release(struct element_t *);
void bpp_measurement_report(struct element_t *);

#endif // _ELEMENTS_BPP_MEASUREMENT_H_
