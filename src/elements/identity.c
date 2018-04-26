#include "memory.h"
#include "element.h"
#include "elements/identity.h"

struct identity_t *identity_create(void) {
    struct identity_t *identity = (struct identity_t *)mem_alloc(sizeof(struct identity_t));

    identity->element.process = identity_process;
    identity->element.release = identity_release;
    identity->element.report = identity_report;
    identity->element.connect = element_connect;
    identity->element.disconnect = element_disconnect;
    identity->element.hop_at = element_hop_at;

    return identity;
}

void identity_process(struct element_t *ele, struct packet_t **pkts, packet_index_t size) {
    ((struct identity_t*)ele)->identity_count += size;

    element_dispatch(ele, 0, pkts, size);
}

void identity_release(struct element_t *ele) {
    mem_release(ele);
}

void identity_report(__attribute__((unused)) struct element_t *_) {
    // VOID
}
