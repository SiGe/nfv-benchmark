#ifndef _ELEMENTS_BP_CHECKSUM_H_
#define _ELEMENTS_BP_CHECKSUM_H_

#include "element.h"
#include "rte_prefetch.h"

struct packet_t;

struct bp_checksum_t {
    struct element_t element;

    uint64_t bp_checksum_count;
};

struct bp_checksum_t *bp_checksum_create(void);
void bp_checksum_process(struct element_t *, struct packet_t **, packet_index_t);
void bp_checksum_release(struct element_t *);
void bp_checksum_report(struct element_t *);

#endif // _ELEMENTS_BP_CHECKSUM_H_
