#include "memory.h"
#include "element.h"
#include "elements/timer.h"

struct timer_t *timer_packet_create(void) {
    struct timer_t *timer = (struct timer_t *)mem_alloc(sizeof(struct timer_t));

    timer->element.process = timer_process;
    timer->element.release = timer_release;
    timer->element.report = timer_report;
    timer->element.connect = element_connect;
    timer->element.disconnect = element_disconnect;
    timer->element.hop_at = element_hop_at;

    return timer;
}

void timer_process(struct element_t *ele, struct packet_t **pkts, packet_index_t size) {
    uint64_t time = rte_get_tsc_cycles();
    for (int i = 0; i < size; ++i) {
        pkts[i]->arrival = time;
    }
    element_dispatch(ele, 0, pkts, size);
}

void timer_release(struct element_t *ele) {
    mem_release(ele);
}

void timer_report(__attribute__((unused)) struct element_t *_) {
    // VOID
}
