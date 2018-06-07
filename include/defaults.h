#ifndef _DEFAULTS_H_
#define _DEFAULTS_H_

#define ELEMENT_MAX_PORT_COUNT     4 // max port count per element
#define ELEMENT_MAX_PORT_COUNT_LOG 2 // max port count per element (logarithm)
#define MAX_PIPELINE_SIZE 128

#define CACHE_LINE_SIZE 64
#define DEBUG_TIME 1
// #undef DEBUG_TIME

/* Conditional variables */
#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1024
#endif

#ifndef PACKET_SIZE
#define PACKET_SIZE 64
#endif

#ifndef REPEAT
#define REPEAT 20
#endif

/* Per element defaults */
#ifndef ROUTING_BUFFER_SIZE
#define ROUTING_BUFFER_SIZE 32
#endif

#ifndef CHECKSUM_BUFFER_SIZE
#define CHECKSUM_BUFFER_SIZE 32
#endif

#ifndef IDENITTY_BUFFER_SIZE
#define IDENTITY_BUFFER_SIZE 32
#endif

#ifndef DROP_BUFFER_SIZE
#define DROP_BUFFER_SIZE 32
#endif

#ifndef MOD_BUFFER_SIZE_DROP
#define MOD_BUFFER_SIZE_DROP DROP_BUFFER_SIZE
#endif


#ifndef MOD_BUFFER_SIZE_1
//#define MOD_BUFFER_SIZE_1 32
#endif

#ifndef MOD_BUFFER_SIZE_2
//#define MOD_BUFFER_SIZE_2 32
#endif

#ifndef MOD_BUFFER_SIZE_3
#define MOD_BUFFER_SIZE_3 32
#endif

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1024
#endif

#ifndef MEASUREMENT_BUFFER_SIZE
#define MEASUREMENT_BUFFER_SIZE 32
#endif

// Histogram parameters
#ifndef HIST_PRECISION
#define HIST_PRECISION 0
#endif

#ifndef HIST_SIZE
#define HIST_SIZE 4096
#endif



#endif //_DEFAULTS_H_
