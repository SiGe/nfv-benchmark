#include "defaults.h"

#include "elements/buffered_element.h"
#include "elements/bypass_element.h"
#include "elements/checksum.h"
#include "elements/drop.h"
#include "elements/drop_mbuf.h"
#include "elements/identity.h"
#include "elements/measurement.h"
#include "elements/routing.h"
#include "elements/timer.h"

#include "benchmark_helper.h"

//----------------------------------------
// Elements setup and creation
//----------------------------------------
struct element_t *el_only_identity_create(void) {
    return (struct element_t*)identity_create();
}

struct element_t *el_only_timer_create(void) {
    return (struct element_t*)timer_packet_create();
}

struct element_t *el_only_drop_create(void) {
    return (struct element_t*)drop_create();
}

inline struct element_t *el_only_checksum_create(void) {
    return (struct element_t*)checksum_create();
}

inline struct element_t *el_only_routing_create(void) {
    struct routing_t *router = routing_create();
    ipv4_prefix_t prefix = { .ipv4=0, .mask=0 };

    routing_entry_add(router, &prefix, 0);
    return (struct element_t*)router;
}

inline struct element_t *el_only_routing_create_with_file(char const *fname) {
    struct routing_t *router = routing_create();
    routing_file_load(router, fname);

    return (struct element_t*)router;
}

inline struct element_t *el_only_measurement_create_with_size(size_t tbl_size) {
    struct measurement_t *monitor = measurement_create();
    measurement_resize(monitor, tbl_size);

    return (struct element_t*)monitor;
}

inline struct element_t *el_only_drop_mbuf_create(struct rx_packet_stream* stream) {
    struct drop_mbuf_t *drop = drop_mbuf_create(stream);
    return (struct element_t*)drop;
}

//----------------------------------------
// Buffered elements creation
//----------------------------------------
inline struct element_t *el_timer_create(packet_index_t size) {
    return (struct element_t *)buffered_element_create(
            (struct element_t*)el_only_timer_create(), size);
}

inline struct element_t *el_identity_create(packet_index_t size) {
    return (struct element_t *)buffered_element_create(
            (struct element_t*)el_only_identity_create(), size);
}

inline struct element_t *el_drop_create(packet_index_t size) {
    return (struct element_t *)buffered_element_create(
            (struct element_t*)el_only_drop_create(), size);
}

inline struct element_t *el_checksum_create(packet_index_t size) {
    return (struct element_t *)buffered_element_create(
            (struct element_t*)el_only_checksum_create(), size);
}

inline struct element_t *el_routing_create(packet_index_t size) {
    return (struct element_t *)buffered_element_create(
            (struct element_t*)el_only_routing_create(), size);
}

inline struct element_t *el_routing_create_with_file(
        packet_index_t size, char const *fname) {
    return (struct element_t *)buffered_element_create(
            (struct element_t*)el_only_routing_create_with_file(fname), size);
}

inline struct element_t *el_measurement_create_with_size(
        packet_index_t size, size_t tbl_size) {
    return (struct element_t *)buffered_element_create(
            (struct element_t*)el_only_measurement_create_with_size(tbl_size), size);
}

inline struct element_t *el_drop_mbuf_create(
        packet_index_t size, struct rx_packet_stream* stream) {
    return (struct element_t *)buffered_element_create(
            (struct element_t*)el_only_drop_mbuf_create(stream), size);
}

inline struct element_t *el_timer_create(packet_index_t size) {
    return (struct element_t *)buffered_element_create(
            (struct element_t*)el_only_timer_create(), size);
}

//----------------------------------------
// Bypass elements creation
//----------------------------------------
inline struct element_t *el_identity_create(packet_index_t size) {
    return (struct element_t *)bypass_element_create(
            (struct element_t*)el_only_identity_create(), size);
}

inline struct element_t *el_drop_create(packet_index_t size) {
    return (struct element_t *)bypass_element_create(
            (struct element_t*)el_only_drop_create(), size);
}

inline struct element_t *el_checksum_create(packet_index_t size) {
    return (struct element_t *)bypass_element_create(
            (struct element_t*)el_only_checksum_create(), size);
}

inline struct element_t *el_routing_create(packet_index_t size) {
    return (struct element_t *)bypass_element_create(
            (struct element_t*)el_only_routing_create(), size);
}

inline struct element_t *el_routing_create_with_file(
        packet_index_t size, char const *fname) {
    return (struct element_t *)bypass_element_create(
            (struct element_t*)el_only_routing_create_with_file(fname), size);
}

inline struct element_t *el_measurement_create_with_size(
        packet_index_t size, size_t tbl_size) {
    return (struct element_t *)bypass_element_create(
            (struct element_t*)el_only_measurement_create_with_size(tbl_size), size);
}

inline struct element_t *el_drop_mbuf_create(
        packet_index_t size, struct rx_packet_stream* stream) {
    return (struct element_t *)bypass_element_create(
            (struct element_t*)el_only_drop_mbuf_create(stream), size);
}



