#include "memory.h"

#include <malloc.h>


void *mem_alloc(size_t size) {
    return malloc(size);
}

void *mem_alloc_align(size_t align, size_t size) {
    return memalign(align, size);
}

void mem_release(void *loc) {
    free(loc);
}

