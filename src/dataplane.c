#include <stdint.h>
#include <unistd.h>

#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)

#include "rte_cycles.h"
#include "rte_ethdev.h"
#include "rte_malloc.h"
#include "rte_mempool.h"

#include "memory.h"
#include "dataplane.h"

#define GAP "\t\t"

int g_record_time = 0;

void dataplane_rebuffer_tx_callback(struct rte_mbuf **mbufs, uint16_t unsent, void *arg) {
    struct rte_eth_dev_tx_buffer *buffer = ((struct dataplane_port_t*)arg)->tx_buffer;
    uint16_t port = ((struct dataplane_port_t*)arg)->port_id;

    for (uint16_t i = 0; i < unsent; ++i)
        rte_eth_tx_buffer(port, 0, buffer, mbufs[i]);
}

static struct rte_eth_conf PORT_CONFIG_DEFAULT = {
	.rxmode = {
		.split_hdr_size = 0,
		.header_split   = 0, /**< Header Split disabled */
		.hw_ip_checksum = 0, /**< IP checksum offload disabled */
		.hw_vlan_filter = 0, /**< VLAN filtering disabled */
		.jumbo_frame    = 0, /**< Jumbo Frame Support disabled */
		.hw_strip_crc   = 0, /**< CRC stripped by hardware */
        .mq_mode        = ETH_MQ_RX_RSS,
	},
    .rx_adv_conf = {
        .rss_conf = {
            //.rss_hf = ETH_RSS_TCP | ETH_RSS_UDP
            .rss_hf = ETH_RSS_IP,
        },
    },
	.txmode = {
		.mq_mode = ETH_MQ_TX_NONE,
	},
};

int port_configure(char const *port_name, struct dataplane_port_t **ppport) {
    int ret = 0;
    uint16_t port_id = 0;

    if ((ret = rte_eth_dev_get_port_by_name(port_name, &port_id)))
        return ret;

    *ppport = mem_alloc(sizeof(struct dataplane_port_t));
    struct dataplane_port_t *port = *ppport;

    /* Set rx and tx queues to 1 */
    port->nrxq = 1;
    port->ntxq = 1;
	port->conf = &PORT_CONFIG_DEFAULT;
    port->port_id = port_id;

    /* Get device info */
    struct rte_eth_dev_info dev_info;
    rte_eth_dev_info_get(port->port_id, &dev_info);

    /* Setup port configuration */
	log_info_fmt("Configuring port : %s [%d]", port_name, port_id);
    ret = rte_eth_dev_configure(port_id, port->nrxq, port->ntxq, port->conf);
    if (ret < 0) 
        goto cleanup;

    port->rx_desc = RX_DESC;
    port->tx_desc = TX_DESC;

    /* ---- */
    ret = rte_eth_dev_adjust_nb_rx_tx_desc(port_id, &port->rx_desc, &port->tx_desc);
    if (ret < 0)
        goto cleanup;

    /* Gonna ignore the mac address in this iteration
     * TODO: Prob need to handle it when doing some routing logic */
    // rte_eth_macaddr_get;

    /* Setup port RX buffer */
    uint16_t pool_size = RTE_MAX(1U * (port->rx_desc + port->tx_desc + MAX_PKT_BURST +
                MEMPOOL_CACHE_SIZE * 1U), 8192U);

	log_info_fmt(GAP "Creating RX packet pool: %s [%d]", port_name, port_id);
    port->pool = rte_pktmbuf_pool_create(
            port_name, /* Pool name */
            pool_size,
            MEMPOOL_CACHE_SIZE, 0, 
            RTE_MBUF_DEFAULT_BUF_SIZE,
            rte_socket_id());

    if (port->pool == 0)
        goto cleanup;

    ret = rte_eth_rx_queue_setup(
            port->port_id,
            0, port->rx_desc,
            rte_socket_id(),
            0, port->pool);
    if (ret < 0)
        goto cleanup;
	log_info_fmt(GAP "RX queue is setup: %s@0 [%d]", port_name, port_id);


	log_info_fmt(GAP "Setting up TX queue: %s@0 [%d]", port_name, port_id);
    /* Setup port TX buffer */
    dev_info.default_txconf.txq_flags = ETH_TXQ_FLAGS_IGNORE;
    ret = rte_eth_tx_queue_setup(
            port->port_id,
            0, port->tx_desc,
            rte_eth_dev_socket_id(port->port_id), &dev_info.default_txconf);
    if (ret < 0)
        goto cleanup;
            
	log_info_fmt(GAP "Creating Zmalloc socket: %s@0 [%d]", port_name, port_id);
    log_info_fmt(GAP "Allocating %d bytes.", RTE_ETH_TX_BUFFER_SIZE(MAX_PKT_BURST));
    port->tx_buffer = rte_zmalloc_socket(0,
            RTE_ETH_TX_BUFFER_SIZE(MAX_PKT_BURST), 0,
            rte_eth_dev_socket_id(port->port_id));
    
    log_info_fmt(GAP "Reserved memory on: %d socket.", rte_eth_dev_socket_id(port->port_id));
    if (port->tx_buffer == 0)
            goto cleanup;
    rte_eth_tx_buffer_init(port->tx_buffer, MAX_PKT_BURST);
    rte_eth_tx_buffer_set_err_callback(port->tx_buffer, dataplane_rebuffer_tx_callback, port);
    log_info_fmt(GAP "TX queue is setup: %s@0 [%d]", port_name, port_id);

    port->tx_pool = rte_mempool_create(
            NULL, 
            ge_pow2_32(MAX_PKT_BURST * 64) - 1,
            TX_ELE_SIZE,
            0,
            sizeof(struct rte_pktmbuf_pool_private),
            rte_pktmbuf_pool_init,
            NULL,
            rte_pktmbuf_init, 0, // <-- create mpool for pcap here?
            rte_lcore_to_socket_id(rte_lcore_id()),
            MEMPOOL_F_SP_PUT | MEMPOOL_F_SC_GET);
    if (port->tx_pool == 0)
        rte_exit(EXIT_FAILURE, "Failed to allocate the mempool buffer");


    ret = rte_eth_dev_start(port->port_id);
    if (ret < 0)
        goto cleanup;

    rte_eth_promiscuous_enable(port->port_id);
    return 0;

cleanup:
    port_release(port);
    return ret;
}

int port_release(struct dataplane_port_t *port) {
    rte_eth_dev_stop(port->port_id);
    rte_eth_dev_close(port->port_id);
    mem_release(port);
    return 0;
}

void dataplane_read_stats(struct dataplane_port_t *port) {
	rte_memcpy(&port->port_stats_prev, &port->port_stats_now, sizeof(struct rte_eth_stats));
    struct rte_eth_stats *port_stats = &port->port_stats_now;

    if (rte_eth_stats_get(port->port_id, port_stats))
        rte_exit(EXIT_FAILURE, "Failed to read dataplane stats.");

	uint64_t curtime = rte_rdtsc();
	port->port_stats_time_delta = curtime - port->port_stats_time;
	port->port_stats_time = curtime;
}

static double cycles_to_seconds(uint64_t cycles) {
    return cycles/((double)rte_get_tsc_hz());
}

void dataplane_print_epoch_stats(struct dataplane_port_t *port) {
    struct rte_eth_stats *ps     = &port->port_stats_now;
    struct rte_eth_stats *psp    = &port->port_stats_prev;
    uint64_t delta = port->port_stats_time_delta;

    uint64_t opktsdelta = ps->opackets - psp->opackets;
    uint64_t ipktsdelta = ps->ipackets - psp->ipackets;

    printf(
".---------------------------------------------------------------------------------------------------------.\n"
"| Port (%1d) - MAC [%02X:%02X:%02X:%02X:%02X:%02X]                                                                      |\n"
"|---------------------------------------------------------------------------------------------------------|\n"
"| Out: %15" PRIu64 " | Out bytes: %20" PRIu64 " | Error: %15" PRIu64 " | Rate: %15.2f |\n"
"|---------------------------------------------------------------------------------------------------------|\n"
"| In : %15" PRIu64 " |  In bytes: %20" PRIu64" | Missed: %14" PRIu64 " | Rate: %15.2f | Error: %15" PRIu64 " |\n"
"*---------------------------------------------------------------------------------------------------------*\n",
            port->port_id,
            // port->port_mac.addr_bytes[0], port->port_mac.addr_bytes[1],
            // port->port_mac.addr_bytes[2], port->port_mac.addr_bytes[3],
            // port->port_mac.addr_bytes[4], port->port_mac.addr_bytes[5],
			0, 0, 0, 0, 0, 0,
            ps->opackets, ps->obytes, ps->oerrors, opktsdelta/cycles_to_seconds(delta),
            ps->ipackets, ps->ibytes, ps->imissed, ipktsdelta/cycles_to_seconds(delta), ps->ierrors);
}
