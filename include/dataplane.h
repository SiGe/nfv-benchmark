#ifndef _DATAPLANE_H_
#define _DATAPLANE_H_

#include "packets.h"
#include "rte_ethdev.h"
#include "log.h"

#define RX_DESC 1024
#define TX_DESC 1024
#define MEMPOOL_CACHE_SIZE 256
#define MAX_PKT_BURST 32

struct dataplane_port_t {
    uint16_t    port_id;
    uint16_t    socket_id;
	uint16_t	core_id;

    uint8_t    nrxq; /* Num RX queues */
    uint8_t    ntxq; /* Num TX queues */

    uint16_t         rx_burst_size; /* Burst size of RX queue */
    struct rte_mbuf *rx_burst;

    uint16_t    tx_burst_size; /* Burst size of TX queue */

    uint16_t    rx_desc; /* RX and TX descriptors */
    uint16_t    tx_desc;

    struct rte_eth_conf *conf;  /* Ethernet config */
    struct rte_mempool  *pool;  /* DPDK pktmbuf pool */

    struct rte_eth_tx_buffer *tx_buffer; /* buffer for sending packets? */

    struct rte_eth_stats port_stats_prev; /* Port statistics */
    struct rte_eth_stats port_stats_now;  /* Port statistics */
    uint64_t             port_stats_time; /* Time when the port stat snapshot was taken */
    uint64_t             port_stats_time_delta; /* Time when the port stat snapshot was taken */
};

int port_configure(char const *port_name, struct dataplane_port_t **port);

packet_index_t dataplane_next_batch (struct packet_pool_t *, packet_t **, packet_index_t);
void dataplane_rebuffer_tx_callback(struct rte_mbuf **, uint16_t, void *);
void dataplane_read_stats(struct dataplane_port_t *);
void dataplane_print_epoch_stats(struct dataplane_port_t *);

#endif // _DATAPLANE_H_

