#ifndef _BENCHMARK_HELPER_H_
#define _BENCHMARK_HELPER_H_

#include "types.h"
#include "dataplane.h"
#include "packets.h"

struct element_t;

struct element_t *el_identity_create(packet_index_t size);
struct element_t *el_timer_create(packet_index_t size);
struct element_t *el_drop_create(packet_index_t size);
struct element_t *el_checksum_create(packet_index_t size);
struct element_t *el_routing_create(packet_index_t size);
struct element_t *el_routing_create_with_file(packet_index_t size, char const *fname);
struct element_t *el_measurement_create_with_size(packet_index_t size, size_t tbl_size);
struct element_t *el_drop_mbuf_create(packet_index_t size, struct rx_packet_stream *);

#endif // _BENCHMARK_HELPER_H_
