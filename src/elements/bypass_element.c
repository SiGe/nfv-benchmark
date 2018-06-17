#include "element.h"
#include "packets.h"
#include "elements/bypass_element.h"

struct bypass_element_t *bypass_element_create(struct element_t *element, packet_index_t notify_size) {
    size_t ele_size = sizeof(struct bypass_element_t);
    struct bypass_element_t *ele = mem_alloc(ele_size);

    ele->size = 0;
    ele->element = element;
    ele->notify_size  = notify_size;

    ele->self.process = bypass_element_process;
    ele->self.release = bypass_element_release;
    ele->self.report = bypass_element_report;
    ele->self.connect = bypass_element_connect;
    ele->self.disconnect = bypass_element_disconnect;
    ele->self.hop_at = bypass_element_hop_at;

    // Map the bypass element ports to the element ports
    port_t **ports = (port_t**)&element->ports;
    *ports = (port_t*)ele->self.ports[0];

    return ele;
}

void bypass_element_process(element_t *self, packet_t **pkts, packet_index_t size) {
    struct bypass_element_t *ele = (struct bypass_element_t *)self;
    packet_index_t tot = size;
    packet_index_t ns = ele->notify_size;

    while (tot > ns) {
        ele->element->process(ele->element, pkts + (size - tot), ns);
        tot -= ns;
    }

    ele->element->process(ele->element, pkts + (size - tot), tot);
}

void bypass_element_release(element_t *self) {
    struct bypass_element_t *ele = (struct bypass_element_t *)self;

    // Release the element
    ele->element->release(ele->element);

    // Release self
    mem_release(ele);
}

void bypass_element_report(element_t *self) {
    struct bypass_element_t *ele = (struct bypass_element_t *)self;
    ele->element->report(ele->element);
}

void bypass_element_connect(element_t *self, port_index_t port, element_t* next_hop) {
    struct bypass_element_t *ele = (struct bypass_element_t *)self;
    ele->element->connect(ele->element, port, next_hop);
}

void bypass_element_disconnect(element_t *self, port_index_t port) {
    struct bypass_element_t *ele = (struct bypass_element_t *)self;
    ele->element->disconnect(ele->element, port);
}

element_t *bypass_element_hop_at(element_t *self, port_index_t port) {
    struct bypass_element_t *ele = (struct bypass_element_t *)self;
    return ele->element->hop_at(ele->element, port);
}
