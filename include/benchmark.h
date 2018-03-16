#ifndef _BENCHMARK_H_
#define _BENCHMARK_H_

#include "defaults.h"

#include "buffered_element.h"
#include "elements/identity.h"
#include "elements/drop.h"
#include "elements/checksum.h"
#include "elements/routing.h"
#include "pipeline.h"

struct benchmark_t {
    struct pipeline_t *pipeline;
    struct packet_pool_t *pool;
    uint32_t repeat;
};

static struct element_t *el_identity_create(packet_index_t size) {
    return (struct element_t *)buffered_element_create(
            (struct element_t*)identity_create(), size);
}

static struct element_t *el_drop_create(packet_index_t size) {
    return (struct element_t *)buffered_element_create(
            (struct element_t*)drop_create(), size);
}

static struct element_t *el_checksum_create(packet_index_t size) {
    return (struct element_t *)buffered_element_create(
            (struct element_t*)checksum_create(), size);
}

static struct element_t *el_routing_create(packet_index_t size) {
    struct routing_t *router = routing_create();
    ipv4_prefix_t prefix = { ipv4: 0, mask: 0 };

    routing_entry_add(router, &prefix, 0);
    return (struct element_t *)buffered_element_create(
            (struct element_t*)router, size);
}

static struct element_t *el_routing_create_with_file(
        packet_index_t size, char const *fname) {
    struct routing_t *router = routing_create();
    routing_file_load(router, fname);

    return (struct element_t *)buffered_element_create(
            (struct element_t*)router, size);
}

void benchmark_config_init(struct benchmark_t *);

#endif
