/**
 * libary for handling logging in the program.
 */
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define KDEF 0

#define KNRM "\x1B[0m"
#define KRED "\x1B[31m"
#define KGRN "\x1B[32m"
#define KYEL "\x1B[33m"
#define KBLU "\x1B[34m"
#define KMAG "\x1B[35m"
#define KCYN "\x1B[36m"
#define KWHT "\x1B[37m"

#define BBLACK "\033[1;30m"
#define BRED "\033[1;31m"
#define BGREEN "\033[1;32m"
#define BYELLOW "\033[1;33m"
#define BBLUE "\033[1;34m"
#define BPURPLE "\033[1;35m"
#define BCYAN "\033[1;36m"
#define BWHITE "\033[1;37m"

// typedef enum COLOR_ENUM { KNRM = "\x1B[0m", KRED = "\x1B[31m" } COLOR;

typedef enum LOG_LEVEL_ENUM { INFO = 0, ERROR = 1, WARN = 2, DEBUG = 3 } LOG_LEVEL;

void shell_write(const char* color, const char* fmt, ...);

void shell_print(const char* color, const char* fmt, ...);
void logger_init();
void b_log(LOG_LEVEL log_level, const char* fmt, ...);