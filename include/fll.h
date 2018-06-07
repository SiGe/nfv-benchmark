#ifndef _FLL_H_
#define _FLL_H_

/*
 * Frequency locked loop between a slave and a master.
 * 
 * Use AIMD to ensure that there is no packet loss on the slave side.  The
 * algorithm works as follows: 
 *  1) the master starts to send packets at max rate,
 *  2) the slave checks to see if there is packet loss on its side.
 *      If yes, it sends a request to increase the delay between packets (in us)
 *      If not, it sends a request to decrease the interpacket delay.
*/

#define FLL_UPPER 200
#define FLL_LOWER 0
#define FLL_MAGIC 0xbeed5335
#define FLL_THRESHOLD 1000

struct __attribute__((packed)) fll_packet_t {
    uint32_t magic;
    uint32_t delay;
};

struct fll_t {
    uint32_t upper;
    uint32_t lower;
    uint32_t current;
};

inline int fll_is_fll(char *data) {
    struct fll_packet_t *pkt = (struct fll_packet_t*)data;
    return pkt->magic == FLL_MAGIC;
}

inline int fll_master(struct fll_t *fll, int loss, char *buffer) {
    if (fll->lower == fll->current || fll->current == fll->upper || loss < FLL_THRESHOLD)
        return 1;

    if (loss == 0) {
        fll->lower = fll->current;
    } else {
        fll->upper = fll->current;
    }
    fll->current = (fll->upper + fll->lower) >> 1;
    struct fll_packet_t *pkt = (struct fll_packet_t *)buffer;
    pkt->magic = FLL_MAGIC;
    pkt->delay = fll->current;
    return 0;
}

inline int fll_follower(struct fll_t *fll, char *data) {
    struct fll_packet_t *pkt = (struct fll_packet_t*)data;
    rte_delay_us(pkt->delay);
}

#endif // _FLL_H_
