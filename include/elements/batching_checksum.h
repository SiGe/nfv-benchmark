#ifndef _ELEMENTS_BATCHING_CHECKSUM_H_
#define _ELEMENTS_BATCHING_CHECKSUM_H_

#include "element.h"
#include "rte_prefetch.h"

struct packet_t;

struct batching_checksum_t {
    struct element_t element;

    uint64_t batching_checksum_count;
};

struct batching_checksum_t *batching_checksum_create(void);
void batching_checksum_process(struct element_t *, struct packet_t **, packet_index_t);
void batching_checksum_release(struct element_t *);
void batching_checksum_report(struct element_t *);

#endif // _ELEMENTS_BATCHING_CHECKSUM_H_
