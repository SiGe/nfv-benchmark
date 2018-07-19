#ifndef _ELEMENTS_BATCHING_ROUTING_H_
#define _ELEMENTS_BATCHING_ROUTING_H_

#include "element.h"
#include "elements/routing.h"
#include "types.h"

struct batching_routing_t {
    struct element_t element;
    uint64_t _tmp;
    
    struct _routing_tbl_256_t fst_tbl;
};

struct batching_routing_t *batching_routing_create(void);
void batching_routing_process(struct element_t *, struct packet_t **, packet_index_t);
void batching_routing_release(struct element_t *);
void batching_routing_report(struct element_t *);

#endif // _ELEMENTS_BATCHING_ROUTING_H_
