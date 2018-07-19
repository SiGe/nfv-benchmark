#ifndef _ELEMENTS_BPP_ROUTING_H_
#define _ELEMENTS_BPP_ROUTING_H_

#include "element.h"
#include "elements/routing.h"
#include "types.h"

struct bpp_routing_t {
    struct element_t element;
    uint64_t _tmp;
    
    struct _routing_tbl_256_t fst_tbl;
};

struct bpp_routing_t *bpp_routing_create(void);
void bpp_routing_process(struct element_t *, struct packet_t **, packet_index_t);
void bpp_routing_release(struct element_t *);
void bpp_routing_report(struct element_t *);

#endif // _ELEMENTS_BPP_ROUTING_H_
