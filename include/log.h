#ifndef _LOG_H_
#define _LOG_H_

#define LOG_ERR 0
#define LOG_WARN 1
#define LOG_INFO 2

#define LOG_RED "\e[1;31m"
#define LOG_YELLOW "\e[1;33m"
#define LOG_GREEN "\e[1;32m"
#define LOG_DONE "\e[0m"

#ifndef LOG_LEVEL
#define LOG_LEVEL 4
#endif

#define GET_TIME(x) {\
    time_t timer;\
    struct tm* tm_info;\
    time(&timer);\
    tm_info = localtime(&timer);\
    strftime(x, 26, "%Y-%m-%d %H:%M:%S", tm_info); }


void log_info(const char *msg);
void log_warn(const char *msg);
void log_err(const char *msg);

#if LOG_LEVEL > LOG_INFO
#define log_info_fmt(_fmt, ...) {\
    char buffer[255] = {0};\
    char fmt[255] = {0};\
    snprintf(fmt, sizeof(fmt), LOG_GREEN "[INF] [%%s] ::" LOG_DONE " %s\n", _fmt);\
    GET_TIME(buffer);\
    printf(fmt, buffer, ##__VA_ARGS__);\
}
#else
#define log_info_fmt(_fmt, ...) {}
#endif

#define panic(_fmt, ...) {\
    char buffer[255] = {0};\
    char fmt[255] = {0};\
    snprintf(fmt, sizeof(fmt), LOG_RED "[ERR] [%%s] ::" LOG_DONE " %s\n", _fmt);\
    GET_TIME(buffer);\
    printf(fmt, buffer, ##__VA_ARGS__);\
    snprintf(fmt, sizeof(fmt), LOG_RED "[%s:%d]" LOG_DONE " => PANIC\n", __FILE__, __LINE__);\
    rte_exit(EXIT_FAILURE, fmt);\
}

#endif // _LOG_H_
