#include <stdio.h>
#include <assert.h>

#include "memory.h"
#include "rte_prefetch.h"

#include "elements/checksum.h"

static unsigned short checksum(const char *buf, unsigned size) {
	unsigned long long sum = 0;
	const unsigned long long *b = (unsigned long long *) buf;

	unsigned t1, t2;
	unsigned short t3, t4;

	while (size >= sizeof(unsigned long long))
	{
        rte_prefetch0(b+128);
        unsigned long long s = *b++;
        sum += s;
        if (sum < s) sum++;
        size -= 8;
	}

	/* Handle tail less than 8-bytes long */
	buf = (const char *) b;
	if (size & 4)
	{
		unsigned s = *(unsigned *)buf;
		sum += s;
		if (sum < s) sum++;
		buf += 4;
	}

	if (size & 2)
	{
		unsigned short s = *(unsigned short *) buf;
		sum += s;
		if (sum < s) sum++;
		buf += 2;
	}

	if (size)
	{
		unsigned char s = *(unsigned char *) buf;
		sum += s;
		if (sum < s) sum++;
	}

	/* Fold down to 16 bits */
	t1 = sum;
	t2 = sum >> 32;
	t1 += t2;
	if (t1 < t2) t1++;
	t3 = t1;
	t4 = t1 >> 16;
	t3 += t4;
	if (t3 < t4) t3++;

	return ~t3;
}

struct checksum_t *checksum_create() {
    struct checksum_t *checksum = (struct checksum_t *)mem_alloc(sizeof(struct checksum_t));

    checksum->element.process = checksum_process;
    checksum->element.release = checksum_release;
    checksum->element.report = checksum_report;
    checksum->element.connect = element_connect;
    checksum->element.disconnect = element_disconnect;
    checksum->element.hop_at = element_hop_at;

    return checksum;
}

void checksum_process(struct element_t *ele, struct packet_t **pkts, packet_index_t size) {
    struct checksum_t *self = ((struct checksum_t*)ele);

    for (packet_index_t i = 0; i < size; ++i) {
        packet_t *pkt = pkts[i];
        self->checksum_count += checksum(pkt->data, pkt->size);
    }

    element_dispatch(ele, 0, pkts, size);
}

void checksum_release(struct element_t *ele) {
    mem_release(ele);
}

void checksum_report(struct element_t *_) {
    // VOID
}
