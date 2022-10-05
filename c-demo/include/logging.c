#include "logging.h"

void log(const char* fmt, ...) {
  char buffer[512] = {0};
  va_list _args;
  va_start(_args, fmt);
  vsnprintf(buffer, sizeof(buffer), fmt, _args);
  va_end(_args);
  //   sprintf(buffer, "\033[1;31m%s: %s\n\033[0m", fmt);
  //   sprintf(buffer, "\033[1;31m%s: %s\n\033[0m", s);
  //   ssize_t res = write(STDOUT_FILENO, buffer, strlen(buffer));
}