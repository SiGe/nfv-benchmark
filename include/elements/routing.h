#ifndef _ELEMENTS_ROUTING_H_
#define _ELEMENTS_ROUTING_H_

#include "element.h"
#include "types.h"

// Simple IP routing using a trie.  The implementation is similar to RadixTrie
// in Click - the first level has 256 buckets, the levels after have 16 each.
// Since our router has ELEMENT_MAX_PORT_COUNT (4) ports, we can encode each
// level in:
//
// First level: 256 * 16 jump table.  
//  We can have a total of 256 * 4 * 4 * 4 * 4 * 4 tables = 2^8 * 2^(2*5) = 2^18
//  Each entry is - 2 bits (port) + 18 bits (table location) + 1 bit (port valid) + 1 bit (jump valid)
//
struct _routing_tbl_entry_t {
    unsigned int port_valid : 1;
    unsigned int tbl_valid  : 1;
    unsigned int port : ELEMENT_MAX_PORT_COUNT_LOG;
    unsigned int lpm  : 3;
    uint32_t     count;
    //unsigned int _reserved : (32 - 3 - 1 - 1 - ELEMENT_MAX_PORT_COUNT_LOG);
    struct _routing_tbl_16_t *tbl;
};

struct _routing_tbl_16_t {
    // 16 * 4 bytes = 64 bytes (a.k.a. a cache line)
    struct _routing_tbl_entry_t entry[16];
};

struct _routing_tbl_256_t {
    // 16 * 4 bytes = 64 bytes (a.k.a. a cache line)
    struct _routing_tbl_entry_t entry[256];
};

struct routing_t {
    struct element_t element;
    uint64_t _tmp;

    struct _routing_tbl_256_t fst_tbl;
};

struct routing_t *routing_create(void);
void routing_process(struct element_t *, struct packet_t **, packet_index_t);
void routing_release(struct element_t *);
void routing_report(struct element_t *);

void routing_entry_add(struct routing_t *, ipv4_prefix_t const *prefix, port_index_t port);
int routing_file_load(struct routing_t *, char const *);
struct _routing_tbl_entry_t *routing_entry_find(struct routing_t *, ipv4_t );

#endif // _ELEMENTS_ROUTING_H_
