#ifndef _FLL_H_
#define _FLL_H_

#include "log.h"
#include "memory.h"

/*
 * Frequency locked loop between a slave and a master.
*/

/*
 * Master pseudo-code:
 *
 * TX/loop() {
 *      while (true) {
 *          to_send = min(fll.wnd_size - fll.in_flight)
 *      }
 * }
 *
 */

#define FLL_MAGIC    (0xbeed5335)
#define FLL_WND_SIZE (1<<10)

#define FLL(pkt) ((struct fll_packet_t*)(pkt))

struct __attribute__((packed)) fll_packet_t {
    uint32_t magic;
    uint16_t reset;
    uint16_t wnd_size;
    uint64_t seq;
};

struct fll_t {
    /* Sender */
    uint16_t wnd_size;
    uint64_t wnd_s;
    uint64_t wnd_e;

    /* Receiver */
    uint64_t last_ack;
};

static inline void fll_ack(struct fll_t *fll, char *pkt) {
    //log_info_fmt("Got ack: %llu\n", FLL(pkt)->seq);
    fll->wnd_s = FLL(pkt)->seq + 1;
}

static inline uint64_t fll_num_pkts_to_send(struct fll_t *fll) {
    uint64_t end = fll->wnd_s + fll->wnd_size;
    if (end < fll->wnd_e)
        return 0;
    return end - fll->wnd_e;
}

static inline void fll_pkts_sent(struct fll_t *fll, uint16_t cnt) {
    //log_info_fmt("Setting wnd_e to: %llu\n", fll->wnd_e + cnt);
    fll->wnd_e += cnt;
}

static inline int fll_is_fll_pkt(char *data) {
    return (FLL(data)->magic == FLL_MAGIC);
}

static inline int fll_is_fll_ack(char *data) {
    return fll_is_fll_pkt(data) && (FLL(data)->reset == 0);
}

static inline int fll_is_fll_reset(char *data) {
    return fll_is_fll_pkt(data) && (FLL(data)->reset == 1);
}

/* Master receives a FLL ack packet */
static inline int fll_sender_ack(struct fll_t *fll, char *buffer) {
    //log_info_fmt("Reset value: %d\n", FLL(buffer)->reset);
    if (unlikely(FLL(buffer)->reset == 1)) {
        fll->wnd_size = FLL(buffer)->wnd_size;
        fll->wnd_s    = FLL(buffer)->seq;
        fll->wnd_e    = fll->wnd_s;
        //log_info_fmt("Resetting fll data-structure.\n");
        return 0;
    }

    /* update the acknowledgement number */
    fll_ack(fll, buffer);
    return 0;
}

static inline int fll_pkt_ack(struct fll_t *fll, char *buffer, uint32_t received) {
    FLL(buffer)->magic    = FLL_MAGIC;
    FLL(buffer)->wnd_size = fll->wnd_size;
    FLL(buffer)->reset    = 0 ;
    FLL(buffer)->seq      = fll->last_ack = fll->last_ack + received;
    //log_info_fmt("Sending ack: %llu\n", fll->last_ack);
    return 0;
}

static inline void fll_pkt_reset(struct fll_t *fll, char *buffer) {
    FLL(buffer)->magic    = FLL_MAGIC;
    FLL(buffer)->wnd_size = fll->wnd_size;
    FLL(buffer)->reset    = 1 ;
    FLL(buffer)->seq      = 0 ;
}

static inline struct fll_t* fll_create(void) {
    struct fll_t *fll = mem_alloc(sizeof(struct fll_t));
    fll->wnd_size     = FLL_WND_SIZE;
    fll->wnd_s        = 0;
    fll->wnd_e        = 0;
    fll->last_ack     = 0;
    return fll;
}

static inline void fll_release(struct fll_t *fll) {
    mem_release(fll);
}

#endif // _FLL_H_
