#ifndef _ELEMENTS_MERGED_H_
#define _ELEMENTS_MERGED_H_

#include "element.h"

struct packet_t;

struct merged_t {
    /* This block should match routing_t's structure */
    struct element_t element;
    uint64_t _tmp;
    struct _routing_tbl_256_t fst_tbl;
    /* until here */

    // Measurement
    size_t   tbl_size;
    uint32_t _tmp[MEASUREMENT_BUFFER_SIZE];
    uint32_t *tbl;

    // Checksum
};

struct merged_t *merged_create(void);
void merged_process(struct element_t *, struct packet_t **, packet_index_t);
void merged_release(struct element_t *);
void merged_report(struct element_t *);

#endif // _ELEMENTS_IDENTITY_H_
