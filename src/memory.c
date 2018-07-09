#include "memory.h"

#define __DPDK

#ifdef __DPDK
#include <rte_malloc.h>
#endif

#include <stdlib.h>


void *mem_realloc(void *ptr, size_t size) {
#ifndef __DPDK
    return realloc(ptr, size);
#else
    return rte_realloc(ptr, size, 0);
#endif
}

void *mem_alloc(size_t size) {
#ifndef __DPDK
    void *ret = malloc(size);
    memset(ret, 0, size);
    return ret;
#else
    return rte_zmalloc_socket(0, size, 0, 1);
#endif
}

void *mem_alloc_align(size_t align, size_t size) {
    void *addr = 0;
#ifndef __DPDK
    int ret = posix_memalign(&addr, align, size);
    if (ret != 0) return 0;

    memset(addr, 0, size);
#else
    addr = rte_malloc_socket(0, size, align, 1);
#endif
    return addr;
}

void mem_release(void *loc) {
#ifndef __DPDK
    free(loc);
#else
    rte_free(loc);
#endif
}

