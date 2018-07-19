#include "defaults.h"

#include "elements/batching_checksum.h"
#include "elements/batching_measurement.h"
#include "elements/batching_routing.h"
#include "elements/bp_checksum.h"
#include "elements/bp_measurement.h"
#include "elements/bp_routing.h"
#include "elements/bpp_checksum.h"
#include "elements/bpp_measurement.h"
#include "elements/bpp_routing.h"
#include "elements/buffered_element.h"
#include "elements/bypass_element.h"
#include "elements/checksum.h"
#include "elements/drop.h"
#include "elements/drop_mbuf.h"
#include "elements/fastpass.h"
#include "elements/identity.h"
#include "elements/measurement.h"
#include "elements/merged.h"
#include "elements/merged_fastpass.h"
#include "elements/merged_opt.h"
#include "elements/naive_checksum.h"
#include "elements/naive_measurement.h"
#include "elements/naive_routing.h"
#include "elements/routing.h"
#include "elements/timer.h"

#include "benchmark_helper.h"

//----------------------------------------
// Elements setup and creation
//----------------------------------------
struct element_t *el_only_merged_fastpass_create(char const *fname, size_t __attribute__((unused)) tbl_size) {
    struct merged_fastpass_t *merged= merged_fastpass_create(
        1933683066, 1769104744, 265260270);
    struct routing_t *router = (struct routing_t *)merged;
    routing_file_load(router, fname);

    return (struct element_t *)merged;
}

struct element_t *el_only_merged_opt_create(char const *fname, size_t __attribute__((unused)) tbl_size) {
    struct merged_opt_t *merged= merged_opt_create();
    struct routing_t *router = (struct routing_t *)merged;
    routing_file_load(router, fname);

    return (struct element_t *)merged;
}

struct element_t *el_only_merged_create(char const *fname, size_t __attribute__((unused)) tbl_size) {
    struct merged_t *merged= merged_create();
    struct routing_t *router = (struct routing_t *)merged;
    routing_file_load(router, fname);

    return (struct element_t *)merged;
}

struct element_t *el_only_fastpass_create(struct rx_packet_stream* stream) {
    struct element_t *el = (struct element_t*)fastpass_create(
        1933683066, 1769104744, 265260270);
    struct drop_mbuf_t *drop = el_only_drop_mbuf_create(stream);

    // attach a drop_mbuf to port/1
    el->connect(el, 1, (element_t *)drop);

    return el;
}

struct element_t *el_only_identity_create(void) {
    return (struct element_t*)identity_create();
}

struct element_t *el_only_timer_create(void) {
    return (struct element_t*)timer_packet_create();
}

struct element_t *el_only_drop_create(void) {
    return (struct element_t*)drop_create();
}

inline struct element_t *el_only_naive_checksum_create(void) {
    return (struct element_t*)naive_checksum_create();
}

inline struct element_t *el_only_batching_checksum_create(void) {
    return (struct element_t*)batching_checksum_create();
}

inline struct element_t *el_only_bp_checksum_create(void) {
    return (struct element_t*)bp_checksum_create();
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

inline struct element_t *el_only_naive_routing_create_with_file(char const *fname) {
    struct naive_routing_t *router = naive_routing_create();
    routing_file_load(router, fname);

    return (struct element_t*)router;
}

inline struct element_t *el_only_batching_routing_create_with_file(char const *fname) {
    struct batching_routing_t *router = batching_routing_create();
    routing_file_load(router, fname);

    return (struct element_t*)router;
}

inline struct element_t *el_only_bp_routing_create_with_file(char const *fname) {
    struct bp_routing_t *router = bp_routing_create();
    routing_file_load(router, fname);

    return (struct element_t*)router;
}

inline struct element_t *el_only_routing_create_with_file(char const *fname) {
    struct routing_t *router = routing_create();
    routing_file_load(router, fname);

    return (struct element_t*)router;
}

inline struct element_t *el_only_naive_measurement_create_with_size(size_t tbl_size) {
    struct naive_measurement_t *monitor = naive_measurement_create();
    naive_measurement_resize(monitor, tbl_size);

    return (struct element_t*)monitor;
}

inline struct element_t *el_only_batching_measurement_create_with_size(size_t tbl_size) {
    struct batching_measurement_t *monitor = batching_measurement_create();
    batching_measurement_resize(monitor, tbl_size);

    return (struct element_t*)monitor;
}

inline struct element_t *el_only_bp_measurement_create_with_size(size_t tbl_size) {
    struct bp_measurement_t *monitor = bp_measurement_create();
    bp_measurement_resize(monitor, tbl_size);

    return (struct element_t*)monitor;
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
inline struct element_t *el_merged_fastpass_create(packet_index_t size, char const *fname, size_t tbl_size) {
    return (struct element_t *)buffered_element_create(
            (struct element_t*)el_only_merged_fastpass_create(fname, tbl_size), size);
}

inline struct element_t *el_merged_opt_create(packet_index_t size, char const *fname, size_t tbl_size) {
    return (struct element_t *)buffered_element_create(
            (struct element_t*)el_only_merged_opt_create(fname, tbl_size), size);
}

inline struct element_t *el_merged_create(packet_index_t size, char const *fname, size_t tbl_size) {
    return (struct element_t *)buffered_element_create(
            (struct element_t*)el_only_merged_create(fname, tbl_size), size);
}

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

inline struct element_t *el_naive_checksum_create(packet_index_t size) {
    return (struct element_t *)buffered_element_create(
            (struct element_t*)el_only_naive_checksum_create(), size);
}

inline struct element_t *el_batching_checksum_create(packet_index_t size) {
    return (struct element_t *)buffered_element_create(
            (struct element_t*)el_only_batching_checksum_create(), size);
}

inline struct element_t *el_bp_checksum_create(packet_index_t size) {
    return (struct element_t *)buffered_element_create(
            (struct element_t*)el_only_bp_checksum_create(), size);
}

inline struct element_t *el_checksum_create(packet_index_t size) {
    return (struct element_t *)buffered_element_create(
            (struct element_t*)el_only_checksum_create(), size);
}

inline struct element_t *el_routing_create(packet_index_t size) {
    return (struct element_t *)buffered_element_create(
            (struct element_t*)el_only_routing_create(), size);
}

inline struct element_t *el_naive_routing_create_with_file(
        packet_index_t size, char const *fname) {
    return (struct element_t *)buffered_element_create(
            (struct element_t*)el_only_naive_routing_create_with_file(fname), size);
}

inline struct element_t *el_batching_routing_create_with_file(
        packet_index_t size, char const *fname) {
    return (struct element_t *)buffered_element_create(
            (struct element_t*)el_only_batching_routing_create_with_file(fname), size);
}

inline struct element_t *el_bp_routing_create_with_file(
        packet_index_t size, char const *fname) {
    return (struct element_t *)buffered_element_create(
            (struct element_t*)el_only_bp_routing_create_with_file(fname), size);
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

inline struct element_t *el_naive_measurement_create_with_size(
        packet_index_t size, size_t tbl_size) {
    return (struct element_t *)buffered_element_create(
            (struct element_t*)el_only_naive_measurement_create_with_size(tbl_size), size);
}

inline struct element_t *el_batching_measurement_create_with_size(
        packet_index_t size, size_t tbl_size) {
    return (struct element_t *)buffered_element_create(
            (struct element_t*)el_only_batching_measurement_create_with_size(tbl_size), size);
}

inline struct element_t *el_bp_measurement_create_with_size(
        packet_index_t size, size_t tbl_size) {
    return (struct element_t *)buffered_element_create(
            (struct element_t*)el_only_bp_measurement_create_with_size(tbl_size), size);
}

inline struct element_t *el_drop_mbuf_create(
        packet_index_t size, struct rx_packet_stream* stream) {
    return (struct element_t *)buffered_element_create(
            (struct element_t*)el_only_drop_mbuf_create(stream), size);
}

inline struct element_t *el_fastpass_create(
        packet_index_t size, struct rx_packet_stream* stream) {
    return (struct element_t *)buffered_element_create(
            (struct element_t*)el_only_fastpass_create(stream), size);
}

//----------------------------------------
// Bypass elements creation
//----------------------------------------
struct element_t *el_bypass_merged_fastpass_create(packet_index_t size, char const *fname, size_t tbl_size) {
    return (struct element_t *)bypass_element_create(
            (struct element_t*)el_only_merged_fastpass_create(fname, tbl_size), size);
}

struct element_t *el_bypass_merged_opt_create(packet_index_t size, char const *fname, size_t tbl_size) {
    return (struct element_t *)bypass_element_create(
            (struct element_t*)el_only_merged_opt_create(fname, tbl_size), size);
}

struct element_t *el_bypass_merged_create(packet_index_t size, char const *fname, size_t tbl_size) {
    return (struct element_t *)bypass_element_create(
            (struct element_t*)el_only_merged_create(fname, tbl_size), size);
}

inline struct element_t *el_bypass_timer_create(packet_index_t size) {
    return (struct element_t *)buffered_element_create(
            (struct element_t*)el_only_timer_create(), size);
}

inline struct element_t *el_bypass_identity_create(packet_index_t size) {
    return (struct element_t *)bypass_element_create(
            (struct element_t*)el_only_identity_create(), size);
}

inline struct element_t *el_bypass_drop_create(packet_index_t size) {
    return (struct element_t *)bypass_element_create(
            (struct element_t*)el_only_drop_create(), size);
}

inline struct element_t *el_bypass_naive_checksum_create(packet_index_t size) {
    return (struct element_t *)bypass_element_create(
            (struct element_t*)el_only_naive_checksum_create(), size);
}

inline struct element_t *el_bypass_batching_checksum_create(packet_index_t size) {
    return (struct element_t *)bypass_element_create(
            (struct element_t*)el_only_batching_checksum_create(), size);
}

inline struct element_t *el_bypass_bp_checksum_create(packet_index_t size) {
    return (struct element_t *)bypass_element_create(
            (struct element_t*)el_only_bp_checksum_create(), size);
}

inline struct element_t *el_bypass_checksum_create(packet_index_t size) {
    return (struct element_t *)bypass_element_create(
            (struct element_t*)el_only_checksum_create(), size);
}

inline struct element_t *el_bypass_routing_create(packet_index_t size) {
    return (struct element_t *)bypass_element_create(
            (struct element_t*)el_only_routing_create(), size);
}

inline struct element_t *el_bypass_naive_routing_create_with_file(
        packet_index_t size, char const *fname) {
    return (struct element_t *)bypass_element_create(
            (struct element_t*)el_only_naive_routing_create_with_file(fname), size);
}

inline struct element_t *el_bypass_batching_routing_create_with_file(
        packet_index_t size, char const *fname) {
    return (struct element_t *)bypass_element_create(
            (struct element_t*)el_only_batching_routing_create_with_file(fname), size);
}

inline struct element_t *el_bypass_bp_routing_create_with_file(
        packet_index_t size, char const *fname) {
    return (struct element_t *)bypass_element_create(
            (struct element_t*)el_only_bp_routing_create_with_file(fname), size);
}

inline struct element_t *el_bypass_routing_create_with_file(
        packet_index_t size, char const *fname) {
    return (struct element_t *)bypass_element_create(
            (struct element_t*)el_only_routing_create_with_file(fname), size);
}

inline struct element_t *el_bypass_naive_measurement_create_with_size(
        packet_index_t size, size_t tbl_size) {
    return (struct element_t *)bypass_element_create(
            (struct element_t*)el_only_naive_measurement_create_with_size(tbl_size), size);
}

inline struct element_t *el_bypass_batching_measurement_create_with_size(
        packet_index_t size, size_t tbl_size) {
    return (struct element_t *)bypass_element_create(
            (struct element_t*)el_only_batching_measurement_create_with_size(tbl_size), size);
}

inline struct element_t *el_bypass_bp_measurement_create_with_size(
        packet_index_t size, size_t tbl_size) {
    return (struct element_t *)bypass_element_create(
            (struct element_t*)el_only_bp_measurement_create_with_size(tbl_size), size);
}

inline struct element_t *el_bypass_measurement_create_with_size(
        packet_index_t size, size_t tbl_size) {
    return (struct element_t *)bypass_element_create(
            (struct element_t*)el_only_measurement_create_with_size(tbl_size), size);
}

inline struct element_t *el_bypass_drop_mbuf_create(
        packet_index_t size, struct rx_packet_stream* stream) {
    return (struct element_t *)bypass_element_create(
            (struct element_t*)el_only_drop_mbuf_create(stream), size);
}

inline struct element_t *el_bypass_fastpass_create(
        packet_index_t size, struct rx_packet_stream* stream) {
    return (struct element_t *)bypass_element_create(
            (struct element_t*)el_only_fastpass_create(stream), size);
}

// BPP
inline struct element_t *el_only_bpp_checksum_create(void) {
    return (struct element_t*)bpp_checksum_create();
}

inline struct element_t *el_only_bpp_routing_create_with_file(char const *fname) {
    struct bpp_routing_t *router = bpp_routing_create();
    routing_file_load(router, fname);

    return (struct element_t*)router;
}
inline struct element_t *el_only_bpp_measurement_create_with_size(size_t tbl_size) {
    struct bpp_measurement_t *monitor = bpp_measurement_create();
    bpp_measurement_resize(monitor, tbl_size);

    return (struct element_t*)monitor;
}

inline struct element_t *el_bpp_checksum_create(packet_index_t size) {
    return (struct element_t *)buffered_element_create(
            (struct element_t*)el_only_bpp_checksum_create(), size);
}

inline struct element_t *el_bpp_routing_create_with_file(
        packet_index_t size, char const *fname) {
    return (struct element_t *)buffered_element_create(
            (struct element_t*)el_only_bpp_routing_create_with_file(fname), size);
}

inline struct element_t *el_bpp_measurement_create_with_size(
        packet_index_t size, size_t tbl_size) {
    return (struct element_t *)buffered_element_create(
            (struct element_t*)el_only_bpp_measurement_create_with_size(tbl_size), size);
}

inline struct element_t *el_bypass_bpp_checksum_create(packet_index_t size) {
    return (struct element_t *)bypass_element_create(
            (struct element_t*)el_only_bpp_checksum_create(), size);
}

inline struct element_t *el_bypass_bpp_routing_create_with_file(
        packet_index_t size, char const *fname) {
    return (struct element_t *)bypass_element_create(
            (struct element_t*)el_only_bpp_routing_create_with_file(fname), size);
}

inline struct element_t *el_bypass_bpp_measurement_create_with_size(
        packet_index_t size, size_t tbl_size) {
    return (struct element_t *)bypass_element_create(
            (struct element_t*)el_only_bpp_measurement_create_with_size(tbl_size), size);
}

