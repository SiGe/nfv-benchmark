#include "element.h"
#include "memory.h"

#include "pipeline.h"

void pipeline_element_add(
    struct pipeline_t* pipeline, struct element_t *ele) {
    if (pipeline->size > 0) {
        struct element_t *prev = pipeline->elements[pipeline->size - 1];
        prev->connect(prev, 0, ele);
    }

    pipeline->elements[pipeline->size++] = ele;
}

void pipeline_element_insert(
    struct pipeline_t* pipeline, struct element_t *ele, pipeline_index_t pos) {
    if (pos >= pipeline->size) {
        pipeline_element_add(pipeline, ele);
        return;
    }

    struct element_t *tmp = 0;
    for (pipeline_index_t i = pos; i <= pipeline->size; ++i) {
        tmp = pipeline->elements[i];
        pipeline->elements[i] = ele;
        ele->connect(ele, 0, tmp);
        ele = tmp;
    }

    tmp->disconnect(tmp, 0);
    pipeline->size++;
}

void pipeline_element_remove(
    struct pipeline_t* pipeline, struct element_t *ele) {
    pipeline_index_t idx = 0;
    struct element_t *prev = 0;
    for (idx = 0; idx < pipeline->size; ++idx) {
        if (pipeline->elements[idx] == ele) {
            // remove the element from the chain.
            prev->connect(prev, 0, ele->ports[0]);
            pipeline->size--;
            break;
        }
        prev = pipeline->elements[idx];
    }

    for (;idx < pipeline->size; ++idx) {
        pipeline->elements[idx] = pipeline->elements[idx+1];
    }
}

void pipeline_process(
    struct pipeline_t* pipeline, struct packet_t **pkts, packet_index_t size) {
    struct element_t *ele = pipeline->elements[0];
    ele->process(ele, pkts, size);
}

void pipeline_release(struct pipeline_t* pipeline) {
    for (pipeline_index_t idx = 0; idx < pipeline->size; ++idx) {
        struct element_t *ele = pipeline->elements[idx];
        ele->release(ele);
    }
}

struct pipeline_t *pipeline_create(void) {
    return (struct pipeline_t *)mem_alloc(sizeof(struct pipeline_t));
}

