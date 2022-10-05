#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define KNRM "\x1B[0m"
#define KRED "\x1B[31m"
#define KGRN "\x1B[32m"
#define KYEL "\x1B[33m"
#define KBLU "\x1B[34m"
#define KMAG "\x1B[35m"
#define KCYN "\x1B[36m"
#define KWHT "\x1B[37m"
// #define BRED "\033[1;31m"
#define BBLACK "\033[1;30m"
#define BRED "\033[1;31m"
#define BGREEN "\033[1;32m"
#define BYELLOW "\033[1;33m"
#define BBLUE "\033[1;34m"
#define BPURPLE "\033[1;35m"
#define BCYAN "\033[1;36m"
#define BWHITE "\033[1;37m"

// typedef enum COLOR_ENUM { KNRM = "\x1B[0m", KRED = "\x1B[31m" } COLOR;

void shell_write(const char* color, const char* fmt, ...) {
  char buffer[256] = {0};
  char output[512] = {0};
  va_list _args;
  va_start(_args, fmt);
  vsnprintf(buffer, sizeof(buffer), fmt, _args);
  va_end(_args);

  sprintf(output, "%s%s\n" KNRM, color, buffer);
  ssize_t res = write(STDOUT_FILENO, output, strlen(output));
}

int main(int argc, char* argv[]) {
  printf("hooo\n");
  shell_write(BRED, "Hello");
  printf("hooo\n");
  shell_write(BBLACK, "Hello");
  int a = 0;

  char buffer[5] = {0};
  if (fgets(buffer, 5, stdin) != NULL) {
    a = atoi(buffer);
  }
  printf("buffer: %s", buffer);
  printf("a: %d\n", a);

  scanf("%d", &a);
  printf("a: %d\n", a);
  exit(0);
}