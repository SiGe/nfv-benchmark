#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "rte_ethdev.h"
#include "rte_malloc.h"

#include "benchmark.h"
#include "dataplane.h"
#include "fll.h"
#include "jit.h"
#include "log.h"
#include "packets.h"
#include "pipeline.h"

#include "rte_cycles.h"
#include "rte_prefetch.h"

#define RX_BURST 32

#undef REPEAT
#define REPEAT 2000000000

enum {CONSOLE_PRINT=0, CONSOLE_STOP};
#define CONSOLE_FREQ 200
static int console_status = CONSOLE_STOP;

int txer(void *);
int datapath_init(int argc, char **, struct dataplane_port_t **);
void datapath_teardown(struct dataplane_port_t *);

struct fll_t *g_fll = 0;

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

    // Create a zipfian distribution for source/destination ip address
    packets_pool_zipfian(pool, 0, packet_count - 1, 26, 8, 1.1);

    {
        packet_t *pkts[MAX_PKT_BURST] = {0};
        packet_index_t batch_size = 0;
        packet_index_t idx;
        struct rte_eth_dev_tx_buffer *buffer = port->tx_buffer;

        struct rte_eth_link link;
        log_info("Waiting for port to bootup.");
        while (1) {
            rte_eth_link_get_nowait(port->port_id, &link);
            printf(".");
            rte_delay_ms(500);
            fflush(stdout);
            if (link.link_status == ETH_LINK_UP){
                printf("\n");
                break;
            }
        }

        // Benchmark the running time of the jitted test
        // Put a memory barrier for benchmarks
        log_info("Waiting for 2 seconds before sending packets.");
        rte_delay_ms(2000);
        console_status = CONSOLE_PRINT;

        uint64_t npkts = 0;
        asm volatile ("mfence" ::: "memory");
        uint64_t cycles = rte_get_tsc_cycles();
		unsigned repeat = REPEAT;
        struct rte_mbuf *rx_mbufs[RX_BURST];

        struct fll_t *fll = g_fll = fll_create();
        packet_index_t burst_size = MAX_PKT_BURST;

		while (repeat > 0) {
			repeat--;
			while ((batch_size = packets_pool_next_batch(pool, pkts, burst_size)) != 0) {
				for (idx = 0; idx < batch_size; ++idx) {
					packet_t *pkt = pkts[idx];
					npkts += packet_send(port, pkt);
				}
                // TODO: burst_size or npkts?  That's the question
                fll_pkts_sent(fll, npkts);

                npkts = rte_eth_rx_burst(port_id, queue_id, rx_mbufs, RX_BURST);
                if (npkts != 0) {
                    for (int i = 0; i < npkts; ++i) {
                        char *hdr = rte_pktmbuf_mtod(rx_mbufs[i], char *) + 14;
                        if (fll_is_fll_pkt(hdr)) {
                            fll_sender_ack(fll, hdr);
                        }

                        // Release RX buffers
                        rte_pktmbuf_free(rx_mbufs[i]);
                    }
                }

                burst_size = fll_num_pkts_to_send(fll);
                burst_size = (burst_size > MAX_PKT_BURST) ? MAX_PKT_BURST : burst_size;
			}
			packets_pool_reset(pool);
		}

        rte_eth_tx_buffer_flush(port_id, queue_id, buffer);
        asm volatile ("mfence" ::: "memory");
        uint64_t end = rte_get_tsc_cycles();
        console_status = CONSOLE_STOP;
        rte_delay_ms(CONSOLE_FREQ);
        log_info_fmt("num cycles per packet (%.2f)", (float)(end - cycles)/(float)(packet_count * REPEAT));
        rte_free(buffer);
        fll_release(fll);
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
        rte_delay_ms(CONSOLE_FREQ);
        if (console_status == CONSOLE_PRINT) {
            printf("\e[1;1H\e[2J");
            if (g_fll) {
                printf("FLL Stats: \n");
                printf("Send window: [%llu %llu %llu]\n", g_fll->wnd_s, g_fll->wnd_e, g_fll->wnd_s + g_fll->wnd_size);
            }
            dataplane_read_stats(port);
            dataplane_print_epoch_stats(port);
        }
	}

	rte_delay_ms(5000);
	dataplane_read_stats(port);
	dataplane_print_epoch_stats(port);
    datapath_teardown(port);
    return 0;
}
