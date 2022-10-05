#include "logging.h"

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