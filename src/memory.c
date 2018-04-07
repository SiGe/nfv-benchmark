#include "memory.h"

#include <stdlib.h>

void *mem_realloc(void *ptr, size_t size) {
    return realloc(ptr, size);
}

void *mem_alloc(size_t size) {
    void *ret = malloc(size);
    memset(ret, 0, size);
    return ret;
}

void *mem_alloc_align(size_t align, size_t size) {
    void *addr = 0;
    int ret = posix_memalign(&addr, align, size);
    if (ret != 0) return 0;

    memset(addr, 0, size);
    return addr;
}

void mem_release(void *loc) {
    free(loc);
}

