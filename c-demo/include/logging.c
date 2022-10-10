#include "logging.h"

/**
 * write to STD_OUT using write
 * async signal safe
 **/
void shell_write(const char* color, const char* fmt, ...) {
  char buffer[256] = {0};
  char output[512] = {0};
  va_list _args;
  va_start(_args, fmt);
  vsnprintf(buffer, sizeof(buffer), fmt, _args);
  va_end(_args);
  if (!color)
    sprintf(output, "%s\n" KNRM, buffer);
  else
    sprintf(output, "%s%s\n" KNRM, color, buffer);
  write(STDOUT_FILENO, output, strlen(output));
}

/**
 * write to STD_OUT using printf
 * not async signal safe
 **/
void shell_print(const char* color, const char* fmt, ...) {
  time_t current_time;
  struct tm* utcTimeInfo;
  current_time = time(0);
  utcTimeInfo = gmtime(&current_time);
  char timeInfoBuffer[25] = {0};
  char buffer[256] = {0};
  char output[512] = {0};

  strftime(timeInfoBuffer, sizeof(timeInfoBuffer), "%Y-%m-%d %H:%M:%S",
           utcTimeInfo);

  va_list _args;
  va_start(_args, fmt);
  vsnprintf(buffer, sizeof(buffer), fmt, _args);
  va_end(_args);
  if (!color)
    sprintf(output, "%s  INFO  %s\n" KNRM, timeInfoBuffer, buffer);
  else
    sprintf(output, "%s%s  INFO  %s\n" KNRM, color, timeInfoBuffer, buffer);
  printf("%s", output);
  fflush(NULL);
}