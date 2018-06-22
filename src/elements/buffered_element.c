#include "element.h"
#include "packets.h"
#include "elements/buffered_element.h"

struct buffered_element_t *buffered_element_create(
        struct element_t *element,
        packet_index_t notify_size) {
    size_t ele_size = sizeof(struct buffered_element_t) + sizeof(struct packet_t*) * notify_size;
    struct buffered_element_t *ele = mem_alloc(ele_size);

    ele->size = 0;
    ele->element = element;
    ele->notify_size  = notify_size;

    ele->self.process = buffered_element_process;
    ele->self.release = buffered_element_release;
    ele->self.report = buffered_element_report;
    ele->self.connect = buffered_element_connect;
    ele->self.disconnect = buffered_element_disconnect;
    ele->self.hop_at = buffered_element_hop_at;

    // If the element is already connected to some elements, remember the
    // connectivity in the proxy, i.e., the buffered element.
    for (int i = 0; i < ELEMENT_MAX_PORT_COUNT; ++i) {
        ele->self.ports[i] = element->ports[i];
    }

    return ele;
}

void buffered_element_process(
        element_t *self, packet_t **pkts, packet_index_t size) {
    struct buffered_element_t *ele = (struct buffered_element_t *)self;
    packet_index_t cur = 0;
    while (cur < size) {
        if (ele->size == ele->notify_size) {
            ele->element->process(ele->element, ele->buffer, ele->notify_size);
            ele->size = 0;
        }

        // Add packet into the buffer
        ele->buffer[ele->size++] = pkts[cur];
        cur++;
    }
}

void buffered_element_release(element_t *self) {
    struct buffered_element_t *ele = (struct buffered_element_t *)self;
    // Process the remaining packets
    if (ele->size > 0) {
        ele->element->process(ele->element, ele->buffer, ele->size);
        ele->size = 0;
    }

    // Release the element
    ele->element->release(ele->element);

    // Release self
    mem_release(ele);
}

void buffered_element_report(element_t *self) {
    struct buffered_element_t *ele = (struct buffered_element_t *)self;
    ele->element->report(ele->element);
}

void buffered_element_connect(element_t *self, port_index_t port, element_t* next_hop) {
    struct buffered_element_t *ele = (struct buffered_element_t *)self;
    ele->element->connect(ele->element, port, next_hop);
}

void buffered_element_disconnect(element_t *self, port_index_t port) {
    struct buffered_element_t *ele = (struct buffered_element_t *)self;
    ele->element->disconnect(ele->element, port);
}

element_t *buffered_element_hop_at(element_t *self, port_index_t port) {
    struct buffered_element_t *ele = (struct buffered_element_t *)self;
    return ele->element->hop_at(ele->element, port);
}
