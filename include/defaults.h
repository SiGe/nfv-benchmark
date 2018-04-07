#ifndef _DEFAULTS_H_
#define _DEFAULTS_H_

#define ELEMENT_MAX_PORT_COUNT     4 // max port count per element
#define ELEMENT_MAX_PORT_COUNT_LOG 2 // max port count per element (logarithm)
#define MAX_PIPELINE_SIZE 128

#define CACHE_LINE_SIZE 64


/* Conditional variables */
#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1024
#endif

#ifndef PACKET_SIZE
#define PACKET_SIZE 1400
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

#ifndef MOD_BUFFER_SIZE_1
#define MOD_BUFFER_SIZE_1 32
#endif

#ifndef MOD_BUFFER_SIZE_2
#define MOD_BUFFER_SIZE_2 32
#endif

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1024
#endif

#ifndef MEASUREMENT_BUFFER_SIZE
#define MEASUREMENT_BUFFER_SIZE 32
#endif

#endif //_DEFAULTS_H_
