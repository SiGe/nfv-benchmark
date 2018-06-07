#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "rte_ethdev.h"
#include "rte_malloc.h"

#include "benchmark_rxer.h"
#include "benchmark_helper.h"
#include "dataplane.h"
#include "fll.h"
#include "jit.h"
#include "log.h"
#include "packets.h"
#include "pipeline.h"

#include "rte_cycles.h"
#include "rte_prefetch.h"

#undef REPEAT
#define REPEAT 200

#define RX_BURST 256
#define LARGEST_BUFFER (RX_BURST * 16)

enum {CONSOLE_PRINT=0, CONSOLE_STOP};
#define CONSOLE_FREQ 200
static int console_status = CONSOLE_STOP;

int  rxer(void *);
int  datapath_init(int argc, char **, struct dataplane_port_t **);
void datapath_teardown(struct dataplane_port_t *);

/* TODO:
 * Normal vs. DDOS distribution packet size distribution
 */
int rxer(void *arg) {
    struct dataplane_port_t *port = (struct dataplane_port_t *)arg;
	if (rte_lcore_id() != port->core_id) {
		return 0;
	}

    {
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

        log_info("Waiting for 2 seconds before receiving packets.");
        rte_delay_ms(2000);
        console_status = CONSOLE_PRINT;

        const uint16_t port_id = port->port_id;
        const uint16_t queue_id = port->queue_id;
        uint16_t npkts = 0;
        struct rte_mbuf *rx_mbufs[RX_BURST];
        struct packet_t *rx_pkts[RX_BURST];
        struct rx_packet_stream *stream = 0;
        if (rx_stream_create(LARGEST_BUFFER, rte_eth_dev_socket_id(port_id), &stream) < 0)
            rte_exit(EXIT_FAILURE, "Failed to allocate rx_stream buffer.");
        asm volatile ("mfence" ::: "memory");

        struct pipeline_t *pipe = pipeline_build(stream);
        uint64_t rx_time = 0;
        uint64_t rx_diff = 0;

        const uint64_t runs[] = { 1<<24, 1<<24 };
        uint64_t packet_count = 0;
        uint64_t packet_missed = 0;
        struct rte_eth_stats stats;

        for (unsigned run = 0; run < sizeof(runs)/sizeof(uint64_t); ++run) {
            packet_count = 0;
            rx_time = 0;
            rte_eth_stats_get(port_id, &stats);
            packet_missed = stats.imissed;
            if (run > 0)
                g_record_time = 1;

            while (1) {
                npkts = rte_eth_rx_burst(port_id, queue_id, rx_mbufs, RX_BURST);
                if (npkts == 0)
                    continue;
                packet_count += npkts;
                rx_stream_mtop(stream, rx_mbufs, npkts, rx_pkts);
                rx_diff = rte_get_tsc_cycles();
                pipeline_process(pipe, rx_pkts, npkts);
                rx_time += rte_get_tsc_cycles() - rx_diff;
                if (unlikely(packet_count > runs[run]))
                    break;
            }
            rte_eth_stats_get(port_id, &stats);
            packet_missed -= stats.imissed;
        }

        assert(packet_missed == 0);

        pipeline_release(pipe);
        rx_stream_release(stream);
        asm volatile ("mfence" ::: "memory");
        console_status = CONSOLE_STOP;
        rte_delay_ms(CONSOLE_FREQ);
        printf("num cycles per packet (%.2f)\n", (float)(rx_time)/(float)(packet_count));
        rte_free(buffer);
    }

    return 0;
}

int datapath_init(int argc, char **argv, struct dataplane_port_t **port) {
    *port = 0;
    int ret = rte_eal_init(argc, argv);
    if (ret < 0)
        rte_exit(EXIT_FAILURE, "Failed to initialize the EAL.");

    //const char port_name[] = "0000:89:00.2";
    const char port_name[] = "0000:06:00.2";
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

    rte_eal_mp_remote_launch(rxer, (void *)port, CALL_MASTER);
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
            dataplane_read_stats(port);
            dataplane_print_epoch_stats(port);
        }
	}

    rte_delay_ms(200);
    // dataplane_read_stats(port);
    // dataplane_print_epoch_stats(port);
    datapath_teardown(port);
    return 0;
}
