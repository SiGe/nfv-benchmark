#ifndef _ELEMENTS_MERGED_OPT_H_
#define _ELEMENTS_MERGED_OPT_H_

#include "elements/routing.h"
#include "element.h"

struct packet_t;

struct merged_opt_t {
    /* This block should match routing_t's structure */
    struct element_t element;
    uint64_t _tmp_2;
    struct _routing_tbl_256_t fst_tbl;
    /* until here */

    // Measurement
    size_t   tbl_size;
    uint32_t _tmp[MEASUREMENT_BUFFER_SIZE];
    uint32_t *tbl;

    // Checksum
    uint64_t checksum_count;
    uint64_t port_count;
};

struct merged_opt_t *merged_opt_create(void);
void merged_opt_process(struct element_t *, struct packet_t **, packet_index_t);
void merged_opt_release(struct element_t *);
void merged_opt_report(struct element_t *);

#endif // _ELEMENTS_IDENTITY_H_
