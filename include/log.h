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

#endif // _LOG_H_
