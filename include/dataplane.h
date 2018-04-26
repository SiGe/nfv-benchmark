#ifndef _DATAPLANE_H_
#define _DATAPLANE_H_

#include "packets.h"
#include "rte_ethdev.h"
#include "log.h"

#define RX_DESC 1024
#define TX_DESC 1024
#define MEMPOOL_CACHE_SIZE 256
#define MAX_PKT_BURST 256
#define TX_ELE_SIZE 2048
#define TX_POOL_SIZE (MAX_PKT_BURST * TX_ELE_SIZE)


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

#endif // _DATAPLANE_H_

