#include <assert.h>
#include <stdio.h>

#include "defaults.h"
#include "element.h"
#include "memory.h"
#include "packets.h"
#include "rte_prefetch.h"
#include "types.h"

#include "elements/routing.h"

#define PORT_MASK ((1<<ELEMENT_MAX_PORT_COUNT_LOG) - 1)

static struct _routing_tbl_16_t *routing_table_new(void);

static struct _routing_tbl_16_t *routing_table_new(void) {
    struct _routing_tbl_16_t *new = (struct _routing_tbl_16_t *)mem_alloc(sizeof(struct _routing_tbl_16_t));
    memset(new, 0, sizeof(struct _routing_tbl_16_t));
    return new;
}

struct _routing_tbl_entry_t *routing_entry_find(struct routing_t *rt, ipv4_t ipv4) {
    maskv4_t mask = 32;
    mask -= 8;

    /* find the table entry in the largest table */
    struct _routing_tbl_entry_t *loc = &rt->fst_tbl.entry[ipv4 >> mask];
    ipv4 &= ((1 << mask) - 1);

    /* output port */
    struct _routing_tbl_entry_t *ret = 0;
    if (loc->port_valid) {
        ret = loc;
    }

    struct _routing_tbl_16_t *nxt_hop = 0;
    if (loc->tbl_valid) {
        nxt_hop = loc->tbl;
    }

    while (nxt_hop) {
        mask -= 4;
        loc = &nxt_hop->entry[ipv4 >> mask];
        ipv4 &= ((1 << mask) - 1);

        if (loc->port_valid) {
            ret = loc;
        }

        nxt_hop = 0;
        if (loc->tbl_valid) {
            nxt_hop = loc->tbl;
        }
    }

    return ret;
}

static int _parse_ipv4_prefix(char const *line, __attribute__((unused)) size_t len,
                              ipv4_prefix_t *out, port_index_t *outport) {
    int ip_chunks[4] = {0};
    int mask = 0, port = 0;
    out->ipv4 = 0;

    int ret = sscanf(line, "%d.%d.%d.%d/%d:%d\n", 
            &ip_chunks[0], &ip_chunks[1],
            &ip_chunks[2], &ip_chunks[3], &mask, &port);

    if (ret < 0) { return 0; }
    out->ipv4 = \
                (ip_chunks[0] << 24) +
                (ip_chunks[1] << 16) +
                (ip_chunks[2] << 8) +
                (ip_chunks[3] << 0);
    out->mask = mask;
    *outport = port;
    return 1;
}


int routing_file_load(struct routing_t *rt, char const *fname) {
    FILE *f = fopen(fname, "r");
    if (!f) { return 0; }

    size_t read = 0, len = 0;
    ipv4_prefix_t buf;
    port_index_t outport;

    char *lineptr = 0;
    while (-1 != (read = getline(&lineptr, &len, f))) {
        if (_parse_ipv4_prefix(lineptr, len, &buf, &outport)) {
            routing_entry_add(rt, &buf, outport);
        }
    }

    if (lineptr) {
        free(lineptr);
        lineptr = 0;
    }

    fclose(f);
    return 1;
}

void routing_entry_add(struct routing_t *rt, ipv4_prefix_t const *prefix, port_index_t port) {
    maskv4_t mask = prefix->mask;
    struct _routing_tbl_entry_t *loc = 0;

    /* If mask is smaller than 8, just add to the first tbl and be done */
    if (mask <= 8) {
        loc = &rt->fst_tbl.entry[prefix->ipv4 >> (32-mask)];
        for (maskv4_t i = 0; i < (1U << (8-mask)); ++i) {
            /* If the port has not been set before or the longest prefix match
             * is smaller than this one set the LPM to the port/lpm/and
             * port_valid to their right value. */
            if (!loc->port_valid || loc->lpm < (mask)) {
                loc->port = (port & PORT_MASK);
                loc->port_valid = 1;
                loc->lpm = mask;
            }
            /* Move to the next table; */
            loc += 1;
        }

        return;
    }
    

    ipv4_t ipv4 = prefix->ipv4;
    struct _routing_tbl_16_t *next_hop = 0;
    /* If mask is greater than 8 find the correct entry to push the rule into */
    if (mask > 8) {
        loc = &rt->fst_tbl.entry[ipv4 >> 24];
        /* Mask the upper bits of the ipv4 */
        ipv4 = ipv4 & ((1 << 24) - 1);
        /* if tbl is not valid, i.e., no previous rules, create a new one */
        if (!loc->tbl_valid) {
            loc->tbl = routing_table_new();
            loc->tbl_valid = 1;
        }

        /* try to insert the rule into the next table */
        next_hop = loc->tbl;

        /* consume 8 bits off of mask */
        mask -=8;
    }

    maskv4_t remaining_mask = 24;
    while (mask > 4) {
        /* consume 4 bits off of mask */
        remaining_mask -= 4;
        mask -= 4;
        loc = &next_hop->entry[ipv4 >> remaining_mask];
        ipv4 = ipv4 & ((1 << remaining_mask) - 1);
        if (!loc->tbl_valid) {
            loc->tbl = routing_table_new();
            loc->tbl_valid = 1;
        }

        next_hop = loc->tbl;
    }

    /* If mask is smaller than 4 */
    if (mask <= 4) {
        loc = &next_hop->entry[ipv4 >> remaining_mask];
        ipv4 = ipv4 & ((1U << remaining_mask) - 1);
        for (maskv4_t i = 0; i < 1U << (4-mask); ++i) {
            /* If the port has not been set before or the longest prefix match
             * is smaller than this one set the LPM to the port/lpm/and
             * port_valid to their right value. */
            if (!loc->port_valid || loc->lpm < (mask)) {
                loc->port = (port & PORT_MASK);
                loc->port_valid = 1;
                loc->lpm = mask;
            }
            loc += 1;
        }

        return;
    }
}

struct routing_t *routing_create(void) {
    struct routing_t *ret = mem_alloc(sizeof(struct routing_t));
    memset(ret, 0, sizeof(struct routing_t));
    ret->element.process = routing_process;
    ret->element.release = routing_release;
    ret->element.report = routing_report;
    ret->element.connect = element_connect;
    ret->element.disconnect = element_disconnect;
    ret->element.hop_at = element_hop_at;

    return ret;
}

void routing_process(struct element_t *el, struct packet_t **pkts, packet_index_t count) {
    struct routing_t *self = (struct routing_t *)el;
    uint32_t port_count = 0;

    ELEMENT_TIME_START(pkts, count);

    for (packet_index_t i = 0; i < count; ++i) {
        rte_prefetch0(pkts[i]->hdr);
    }

    for (packet_index_t i = 0; i < count; ++i) {
        struct _routing_tbl_entry_t *ent = routing_entry_find(self, *((ipv4_t*)(pkts[i]->hdr+ 14 + 12 + 4)));
        if (ent) {
            port_count += ent->port;
            ent->count ++;
        }
    }

    ELEMENT_TIME_END(pkts, count);

    element_dispatch(el, port_count, pkts, count);
}

void routing_release_tbl(struct _routing_tbl_16_t *tbl) {
    struct _routing_tbl_entry_t *entry = 0;
    for (int i = 0; i < 16; ++i) {
        entry = &tbl->entry[i];
        if (entry->tbl_valid) {
            routing_release_tbl(entry->tbl);
        }
    }
    mem_release(tbl);
}

void routing_release(struct element_t *el) {
    struct _routing_tbl_256_t *fst = &((struct routing_t *)el)->fst_tbl;
    for (int i = 0; i < 256; ++i) {
        if (fst->entry[i].tbl_valid) {
            routing_release_tbl(fst->entry[i].tbl);
        }
    }
    mem_release(el);
}

void routing_report(__attribute__((unused)) struct element_t *_) {
}
