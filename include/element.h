#ifndef _ELEMENT_H_
#define _ELEMENT_H_

#include "defaults.h"
#include "rte_cycles.h"
#include "packets.h"

struct element_t;
typedef struct element_t element_t;

typedef struct element_t port_t;
typedef uint8_t port_index_t;

typedef void (*ele_process_batch)(struct element_t *, packet_t **pkts, packet_index_t size);
typedef void (*ele_release)(struct element_t *);
typedef void (*ele_report)(struct element_t *);
typedef void (*ele_connect)(struct element_t *, port_index_t, struct element_t *);
typedef void (*ele_disconnect)(struct element_t *, port_index_t);
typedef element_t* (*ele_hop_at)(struct element_t *, port_index_t);

struct element_t {
    ele_process_batch process;
    ele_report        report;
    ele_release       release;
    ele_connect       connect;
    ele_disconnect       disconnect;
    ele_hop_at       hop_at;

    port_t *ports[ELEMENT_MAX_PORT_COUNT];
};

void element_dispatch(
        struct element_t *element, port_index_t idx,
        struct packet_t **pkts, packet_index_t size);

void element_connect(
        struct element_t *element, port_index_t port,
        struct element_t *next_hop);

void element_disconnect(
        struct element_t *element, port_index_t port);

element_t *element_hop_at(
        struct element_t *element, port_index_t port);

#ifdef DEBUG_TIME_2
#define ELEMENT_TIME_START(pkts, n) \
    uint64_t __time = rte_get_tsc_cycles();

#define ELEMENT_TIME_END(pkts, n) {\
    __time = (rte_get_tsc_cycles() - __time) / n;\
    for (packet_index_t i = 0; i < n; ++i) {\
        pkts[i]->time += __time;\
    }\
}
#else

#define ELEMENT_TIME_START(pkts, n) {};
#define ELEMENT_TIME_END(pkts, n) {};

#endif // DEBUG_TIME

#endif // _ELEMENT_H_
