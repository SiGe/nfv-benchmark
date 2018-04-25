#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "rte_ethdev.h"

#include "benchmark.h"
#include "dataplane.h"
#include "jit.h"
#include "log.h"
#include "packets.h"
#include "pipeline.h"

#include "rte_cycles.h"
#include "rte_prefetch.h"

#define TX_ELE_SIZE 2048
#define TX_POOL_SIZE (MAX_PKT_BURST * TX_ELE_SIZE)

#define REPEAT 20

/* TODO:
 * Normal vs. DDOS distribution packet size distribution
 */
int txer(void *arg) {
    struct dataplane_port_t *port = (struct dataplane_port_t *)arg;
	if (rte_lcore_id() != port->core_id) {
		return 0;
	}

    uint16_t port_id = port->port_id;
    uint16_t queue_id = 0;

    uint32_t packet_count = 1<<20;

    log_info("Preparing packet pool.");
    struct packet_pool_t *pool = packets_pool_create(packet_count, PACKET_SIZE);
    log_info("Done preparation of packet pool.");

    struct rte_mempool *mbuf_pool = rte_mempool_create(
            NULL, 
            ge_pow2_32(MAX_PKT_BURST * 1024) - 1,
            TX_ELE_SIZE,
            0,
            sizeof(struct rte_pktmbuf_pool_private),
            rte_pktmbuf_pool_init,
            NULL,
            rte_pktmbuf_init, 0, // <-- create mpool for pcap here?
            rte_lcore_to_socket_id(rte_lcore_id()),
            MEMPOOL_F_SP_PUT | MEMPOOL_F_SC_GET);
    if (mbuf_pool == 0)
        rte_exit(EXIT_FAILURE, "Failed to allocate the mempool buffer");

    // Create a zipfian distribution for source/destination ip address
    packets_pool_zipfian(pool, 0, packet_count - 1, 26, 8, 0.5);

    {
        packet_t *pkts[MAX_PKT_BURST] = {0};
        packet_index_t batch_size = 0;
        packet_index_t idx;
        struct rte_mbuf *m;
        struct rte_eth_dev_tx_buffer *buffer = port->tx_buffer;

        /*
        size_t buffer_size = RTE_ETH_TX_BUFFER_SIZE(MAX_PKT_BURST);
        log_info_fmt("Allocated %lu for the buffer", buffer_size);
        struct rte_eth_dev_tx_buffer *buffer = (struct rte_eth_dev_tx_buffer*)
            rte_zmalloc_socket("tx-buffer", buffer_size, 0, rte_eth_dev_socket_id(port));
        if (buffer == 0)
            rte_exit(EXIT_FAILURE, "Failed to allocate tx buffer to send packets out.");

        //rte_eth_tx_buffer_init(buffer, MAX_PKT_BURST);
        buffer->size = MAX_PKT_BURST;
        rte_eth_tx_buffer_set_err_callback(buffer, rte_eth_tx_buffer_drop_callback, NULL);
        */

        // Benchmark the running time of the jitted test
        // Put a memory barrier for benchmarks
        log_info("Preparing to send packets.");
        uint64_t count = 0;
        uint64_t npkts = 0;
        asm volatile ("mfence" ::: "memory");
        uint64_t cycles = rte_get_tsc_cycles();
		unsigned repeat = REPEAT;

		while (repeat > 0) {
			repeat--;
			while ((batch_size = packets_pool_next_batch(pool, pkts, MAX_PKT_BURST)) != 0) {
				for (idx = 0; idx < batch_size; ++idx) {
					packet_t *pkt = pkts[idx];
					m = rte_pktmbuf_alloc(mbuf_pool);
					rte_memcpy((uint8_t*)m->buf_addr + m->data_off, pkt->hdr, pkt->size);
					m->pkt_len = pkt->size;
					m->data_len = pkt->size;

					npkts += rte_eth_tx_buffer(port_id, queue_id, buffer, m);
				}
				count+= batch_size;
			}
			packets_pool_reset(pool);
		}

        rte_free(buffer);
        rte_mempool_free(mbuf_pool);

        asm volatile ("mfence" ::: "memory");
        log_info_fmt("num cycles per packet (%.2f)", (float)(rte_get_tsc_cycles() - cycles)/(float)(packet_count * REPEAT));
    }

    packets_pool_delete(&pool);

    return 0;
}

int datapath_init(int argc, char **argv, struct dataplane_port_t **port) {
    *port = 0;
    int ret = rte_eal_init(argc, argv);
    if (ret < 0)
        rte_exit(EXIT_FAILURE, "Failed to initialize the EAL.");

    const char port_name[] = "0000:89:00.2";
    log_info_fmt("Num available dpdk ports: %d", rte_eth_dev_count());

    struct dataplane_port_t *pport = 0;
    ret = port_configure(port_name, &pport);
    if (ret < 0)
        rte_exit(EXIT_FAILURE, "Failed to configure port %s", port_name);
    log_info_fmt("Port [%s] configured successfully.", port_name);
    *port = pport;
    return ret;
}

void datapath_teardown(struct dataplane_port_t *port) {
    port_release(port);
    rte_eal_cleanup();
}

int main(int argc, char **argv) {
    // Deterministic experiments are the best experiments - one can only hope.
    srand(0);

    struct dataplane_port_t *port = 0;
    int ret = datapath_init(argc, argv, &port);
    argc -= ret;
    argv += ret;

    if (!port) 
        return 0;

	port->core_id = 3;

    rte_eal_mp_remote_launch(txer, (void *)port, CALL_MASTER);
	unsigned lcore_id = 0;
	unsigned finished = 0;

	while (!finished) {
		finished = 1;
		RTE_LCORE_FOREACH_SLAVE(lcore_id) {
			if (rte_eal_get_lcore_state(lcore_id) == RUNNING) {
				finished = 0;
				break;
			}
		}
		sleep(1);
		dataplane_read_stats(port);
		dataplane_print_epoch_stats(port);
	}

	sleep(3);
	dataplane_read_stats(port);
	dataplane_print_epoch_stats(port);
    datapath_teardown(port);
    return 0;
}
