#ifndef _DATAPLANE_H_
#define _DATAPLANE_H_

#include <assert.h>

#include "defaults.h"
#include "packets.h"
#include "rte_cycles.h"
#include "rte_ethdev.h"
#include "rte_malloc.h"
#include "log.h"

extern int g_record_time;

struct dataplane_port_t {
    uint16_t    port_id;
    uint16_t    queue_id;
    uint16_t    socket_id;
	uint16_t	core_id;

    /* Keep these close to port_id for cache efficiency */
    struct rte_mempool  *tx_pool; /* DPDK tx buffer pool */
    struct rte_eth_dev_tx_buffer *tx_buffer; /* buffer for sending packets? */

    uint8_t    nrxq; /* Num RX queues */
    uint8_t    ntxq; /* Num TX queues */

    uint16_t         rx_burst_size; /* Burst size of RX queue */
    struct rte_mbuf *rx_burst;

    uint16_t    tx_burst_size; /* Burst size of TX queue */

    uint16_t    rx_desc; /* RX and TX descriptors */
    uint16_t    tx_desc;

    struct rte_eth_conf *conf;  /* Ethernet config */
    struct rte_mempool  *pool;  /* DPDK pktmbuf pool */

    struct rte_eth_stats port_stats_prev; /* Port statistics */
    struct rte_eth_stats port_stats_now;  /* Port statistics */
    uint64_t             port_stats_time; /* Time when the port stat snapshot was taken */
    uint64_t             port_stats_time_delta; /* Time when the port stat snapshot was taken */
};

int port_configure(char const *port_name, struct dataplane_port_t **port);
int port_release(struct dataplane_port_t *port);

void dataplane_rebuffer_tx_callback(struct rte_mbuf **, uint16_t, void *);
void dataplane_read_stats(struct dataplane_port_t *);
void dataplane_print_epoch_stats(struct dataplane_port_t *);

static inline int packet_send(struct dataplane_port_t *port, packet_t *pkt) {
    struct rte_mbuf *m = rte_pktmbuf_alloc(port->tx_pool);
    rte_memcpy((uint8_t*)m->buf_addr + m->data_off, pkt->hdr, pkt->size);
    m->pkt_len = pkt->size;
    m->data_len = pkt->size;

    return rte_eth_tx_buffer(port->port_id, port->queue_id, port->tx_buffer, m);
};

// An rx_packet_stream has two data structures:
//     A mempool that holds (twice?) as many packets as the largest buffered element in the pipeline
//     A ring that holds free pkt slots 
//
//     struct packet_t **pkts;
//     int n = rx_burst()
//     rte_ring_sc_dequeue_bulk(ring, pkts, n, 0);
//
//     for i : n {
//         pkts[i]->hdr = rte_pktmbuf_mtod(m[i])
//         pkts[i]->payload = ...
//         pkts[i]->size = ...
//         pkts[i]->metadata = mbuf[i]
//     }
//
//     send_into_pipeline()
//
//     drop_mbuf() {
//          for pkt : pkts {
//              rte_pktmbuf_free(pkt->metadata);
//          }
//     }
struct rx_packet_stream {
    struct rte_ring *ring;
    struct packet_t *pkts;
    int size;
    int queue_length;
    uint32_t tmp;

    uint64_t average_queue_length;
    uint64_t packet_count;
};

static inline
int rx_stream_create(size_t size, int socket, struct rx_packet_stream **pstream) {
    *pstream = rte_malloc_socket(0, sizeof(struct rx_packet_stream), 0, socket);
    struct rx_packet_stream *stream = *pstream;

    stream->ring = rte_ring_create(0, size, socket, RING_F_SP_ENQ | RING_F_SC_DEQ);
    stream->pkts = rte_zmalloc_socket(0, sizeof(struct packet_t) * size, 0, socket);
    stream->size = size;
    stream->queue_length = 0;
    stream->packet_count = 0;
    stream->average_queue_length = 0;

    // Enqueue the freespaces
    for (size_t i = 0; i < size; ++i) {
        rte_ring_enqueue(stream->ring, &stream->pkts[i]);
    }
    return 0;
}

static inline
int rx_stream_release(struct rx_packet_stream *stream) {
    rte_ring_free(stream->ring);
    rte_free(stream->pkts);
    rte_free(stream);
    return 0;
}

static inline
int rx_stream_mtop(struct rx_packet_stream *stream, 
        struct rte_mbuf **ms, unsigned int n, packet_t **pkts) {
     if (unlikely(rte_ring_sc_dequeue_bulk(stream->ring, (void **)pkts, n, 0) == 0))
         rte_exit(EXIT_FAILURE, "Failed to dequeue enough packets slots.");

     for (unsigned int i = 0; i < n; ++i) {
         const struct rte_mbuf *mss = ms[i];
         char *pkt = rte_pktmbuf_mtod(mss, char *);
         pkts[i]->hdr = pkt;
         pkts[i]->payload = pkt + 40;
         pkts[i]->size = mss->data_len;
         pkts[i]->metadata = (void*)mss;
         pkts[i]->queue_length = stream->queue_length + i;
     }

     stream->queue_length += n;
     return 0;
}

static inline
int rx_stream_release_pkts(struct rx_packet_stream *stream,
        packet_t **pkts, size_t n, uint32_t *hist) {
     uint64_t idx = 0;

     if (g_record_time) {
         uint64_t time = rte_get_tsc_cycles();
         for (size_t i = 0; i < n; ++i) {
             struct packet_t *pkt = pkts[i];
             rte_pktmbuf_free((struct rte_mbuf*)pkt->metadata);
             rte_ring_sp_enqueue(stream->ring, pkt);
             stream->average_queue_length += pkt->queue_length;
             idx = ((time - pkt->arrival)) >> 4;// / (pkt->queue_length + 1));
             idx = (idx >= HIST_SIZE) ? HIST_SIZE-1 : idx;
             hist[idx]++;
         }
         stream->packet_count += n;

     } else {
         for (size_t i = 0; i < n; ++i) {
             rte_pktmbuf_free((struct rte_mbuf*)pkts[i]->metadata);
             rte_ring_sp_enqueue(stream->ring, pkts[i]);
         }
     }

     stream->queue_length -= n;
     assert(stream->queue_length >= 0);

     return 0;
}


#endif // _DATAPLANE_H_

