#include <assert.h>

#include "element.h"

void element_dispatch(
        struct element_t *element, port_index_t idx,
        struct packet_t **pkts, packet_index_t size) {
    struct element_t *el = element->ports[idx];
    el->process(el, pkts, size);
}

void element_connect(
        struct element_t *element, port_index_t port,
        struct element_t *next_hop) {
    assert(port < ELEMENT_MAX_PORT_COUNT);
    element->ports[port] = next_hop;
}

void element_disconnect(
        struct element_t *element, port_index_t port) {
    assert(port < ELEMENT_MAX_PORT_COUNT);
    element->ports[port] = 0;
}

element_t *element_hop_at(
        struct element_t *element, port_index_t port) {
    assert(port < ELEMENT_MAX_PORT_COUNT);
    return element->ports[port];
}
