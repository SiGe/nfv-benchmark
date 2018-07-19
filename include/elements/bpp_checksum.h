#ifndef _ELEMENTS_BPP_CHECKSUM_H_
#define _ELEMENTS_BPP_CHECKSUM_H_

#include "element.h"
#include "rte_prefetch.h"

struct packet_t;

struct bpp_checksum_t {
    struct element_t element;

    uint64_t bpp_checksum_count;
};

struct bpp_checksum_t *bpp_checksum_create(void);
void bpp_checksum_process(struct element_t *, struct packet_t **, packet_index_t);
void bpp_checksum_release(struct element_t *);
void bpp_checksum_report(struct element_t *);

#endif // _ELEMENTS_BPP_CHECKSUM_H_
