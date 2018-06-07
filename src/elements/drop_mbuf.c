#include <math.h>
#include <rte_cycles.h>
#include "dataplane.h"
#include "memory.h"
#include "elements/drop_mbuf.h"

struct drop_mbuf_t *drop_mbuf_create(struct rx_packet_stream *stream) {
    struct drop_mbuf_t *drop_mbuf = (struct drop_mbuf_t *)mem_alloc(sizeof(struct drop_mbuf_t));

    drop_mbuf->element.process = drop_mbuf_process;
    drop_mbuf->element.release = drop_mbuf_release;
    drop_mbuf->element.report = drop_mbuf_report;
    drop_mbuf->element.connect = element_connect;
    drop_mbuf->element.disconnect = element_disconnect;
    drop_mbuf->element.hop_at = element_hop_at;
    drop_mbuf->rx_stream = stream;
    drop_mbuf->hist = mem_alloc(HIST_SIZE * sizeof(uint32_t));

    return drop_mbuf;
}

void drop_mbuf_process(struct element_t *ele, struct packet_t **pkts, packet_index_t size) {
    struct drop_mbuf_t *e = ((struct drop_mbuf_t*)ele);
    e->drop_count += size;
    rx_stream_release_pkts(e->rx_stream, pkts, size, e->hist);
}

static double
drop_mbuf_average(uint32_t *hist, uint64_t rx_packets) {
    double sum = 0;
    for (int i = 0; i < HIST_SIZE; ++i ){
        sum += (hist[i] * i);
    }
    return (sum / rx_packets) * (1 << HIST_PRECISION);
}

static uint32_t
drop_mbuf_percentile(uint32_t *hist, uint64_t rx_packets, double percentile) {
    double packet_count = (rx_packets * percentile)/ 100.0f;
    uint32_t i = 0;
    uint64_t total_packet = 0;
    for (i = 0; i < HIST_SIZE; ++i) {
        total_packet += hist[i];

        if (total_packet > packet_count)
            return (i) * (1 << HIST_PRECISION);
    }

    return HIST_SIZE * (1 << HIST_PRECISION);
}

static void
drop_mbuf_print_statistics(struct drop_mbuf_t *e) {
    float percentiles[] = {10, 50, 90, 95, 99, 99.9, 99.99, 99.999, 99.9999};
    uint64_t drop_count = 0;
    for (int i = 0; i < HIST_SIZE; ++i ){
        drop_count += e->hist[i];
    }
    for (int i = 0; i < sizeof(percentiles)/sizeof(float); ++i) {
        printf("Percentile\t%.4f\t%d\n", percentiles[i], drop_mbuf_percentile(e->hist, drop_count, percentiles[i]));
    }

    printf("Percentile\tAVG\t%.2f\n", drop_mbuf_average(e->hist, drop_count));
    printf("HIST Precision:%d\t", HIST_PRECISION);
    for (int i = 0; i < HIST_SIZE; ++i ){
        printf("%d\t", e->hist[i]);
    }
    printf("\n");
}

void drop_mbuf_release(struct element_t *ele) {
    struct drop_mbuf_t *e = ((struct drop_mbuf_t*)ele);
#ifdef DEBUG_TIME
    drop_mbuf_print_statistics(e);
#endif // DEBUG_TIME
    mem_release(e->hist);
    mem_release(ele);
}

void drop_mbuf_report(__attribute__((unused)) struct element_t *_) {
    // VOID
}
