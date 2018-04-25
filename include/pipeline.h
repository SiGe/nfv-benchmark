#ifndef _PIPELINE_H_
#define _PIPELINE_H_

#include "defaults.h"
#include "element.h"
#include "packets.h"

typedef uint8_t pipeline_index_t;

struct pipeline_t {
    struct element_t *elements[MAX_PIPELINE_SIZE];

    pipeline_index_t size;
};

void pipeline_element_add(struct pipeline_t* pipeline, struct element_t *ele);
void pipeline_element_insert(struct pipeline_t* pipeline, struct element_t *ele, pipeline_index_t pos);
void pipeline_element_remove(struct pipeline_t* pipeline, struct element_t *ele);

void pipeline_process(struct pipeline_t* pipeline, struct packet_t **pkts, packet_index_t size);
void pipeline_release(struct pipeline_t* pipeline);
struct pipeline_t *pipeline_create(void);

#endif // _PIPELINE_H_
