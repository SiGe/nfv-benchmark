#include <stdio.h>
#include <time.h>

#include "log.h"

inline void log_info(const char *msg) {
#if LOG_LEVEL > LOG_INFO
    char buffer[26] = {0};
    GET_TIME(buffer);
    fprintf(stderr, LOG_GREEN "[INF] [%s] ::" LOG_DONE " %s\n", buffer, msg);
#endif
}


inline void log_warn(const char *msg) {
#if LOG_LEVEL > LOG_WARN
    char buffer[26] = {0};
    GET_TIME(buffer);
    fprintf(stderr, LOG_YELLOW "[WRN] [%s] ::" LOG_DONE " %s\n", buffer, msg);
#endif
}

inline void log_err(const char *msg) {
#if LOG_LEVEL > LOG_ERR
    char buffer[26] = {0};
    GET_TIME(buffer);
    fprintf(stderr, LOG_RED "[ERR] [%s] ::" LOG_DONE " %s\n", buffer, msg);
#endif
}
