#include "memory.h"
#include "elements/drop.h"

struct drop_t *drop_create() {
    struct drop_t *drop = (struct drop_t *)mem_alloc(sizeof(struct drop_t));

    drop->element.process = drop_process;
    drop->element.release = drop_release;
    drop->element.report = drop_report;
    drop->element.connect = element_connect;
    drop->element.disconnect = element_disconnect;
    drop->element.hop_at = element_hop_at;

    return drop;
}

void drop_process(struct element_t *ele, struct packet_t **pkts, packet_index_t size) {
    ((struct drop_t*)ele)->drop_count += size;
}

void drop_release(struct element_t *ele) {
    mem_release(ele);
}

void drop_report(struct element_t *_) {
    // VOID
}
