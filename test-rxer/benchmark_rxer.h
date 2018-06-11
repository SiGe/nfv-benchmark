#ifndef _BENCHMARK_RXER_H_
#define _BENCHMARK_RXER_H_

#include "packets.h"
#include "pipeline.h"

struct rx_packet_stream;

struct pipeline_t *pipeline_build(struct rx_packet_stream *);

#endif // _BENCHMARK_RXER_H_
