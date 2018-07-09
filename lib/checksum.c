#include "rte_branch_prediction.h"
#include "rte_prefetch.h"

#include "checksum.h"

unsigned short checksum(const char *buf, unsigned size) {
	unsigned long long sum = 0;
	const unsigned long long *b = (const unsigned long long *) buf;

	unsigned t1, t2;
	unsigned short t3, t4;

	while (size >= sizeof(unsigned long long))
	{
        //rte_prefetch0(b+128);
        unsigned long long s = *b++;
        sum += s;
        if (sum < s) sum++;
        size -= 8;
	}

	/* Handle tail less than 8-bytes long */
	buf = (const char *) b;
	if (size & 4)
	{
		unsigned s = *(const unsigned *)buf;
		sum += s;
		if (sum < s) sum++;
		buf += 4;
	}

	if (size & 2)
	{
		unsigned short s = *(const unsigned short *) buf;
		sum += s;
		if (sum < s) sum++;
		buf += 2;
	}

	if (size)
	{
		unsigned char s = *(const unsigned char *) buf;
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
