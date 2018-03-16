#ifndef _MEMORY_H_
#define _MEMORY_H_

#include <string.h>

void *mem_alloc(size_t);
void *mem_alloc_align(size_t, size_t);
void *mem_realloc(void *, size_t);
void mem_release(void *);

#endif
