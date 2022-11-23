#include "logging.h"

FILE* fp;
FILE* csv_fp;

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
  strftime(timeInfoBuffer, sizeof(timeInfoBuffer), "%Y-%m-%d %H:%M:%S", utcTimeInfo);

  char buffer[256] = {0};
  char output[512] = {0};

  va_list _args;
  va_start(_args, fmt);
  vsnprintf(buffer, sizeof(buffer), fmt, _args);
  va_end(_args);
  if (!color)
    sprintf(output, "%s  INFO  %s\n" KNRM, timeInfoBuffer, buffer);
  else
    sprintf(output, "%s%s  INFO  %s\n" KNRM, color, timeInfoBuffer, buffer);
  printf("%s", output);
  fflush(stdout);
}

void logger_init() {
  time_t current_time;
  struct tm* utcTimeInfo;
  current_time = time(0);
  utcTimeInfo = gmtime(&current_time);
  char dateInfoBuffer[25] = {0};
  char path[40] = {0};
  strftime(dateInfoBuffer, sizeof(dateInfoBuffer), "%Y-%m-%d", utcTimeInfo);
  sprintf(path, "./log/%s.log", dateInfoBuffer);
  fp = fopen(path, "a+");
}
void logger_close() { fclose(fp); }

/**
 * write to STD_OUT using write
 * async signal safe
 **/
void b_log(LOG_LEVEL log_level, const char* fmt, ...) {
  time_t current_time;
  struct tm* utcTimeInfo;
  current_time = time(0);
  utcTimeInfo = gmtime(&current_time);
  char timeInfoBuffer[25] = {0};
  strftime(timeInfoBuffer, sizeof(timeInfoBuffer), "%Y-%m-%d %H:%M:%S", utcTimeInfo);
  const char* color = {0};
  const char* level = {0};
  switch (log_level) {
    case INFO:
      color = KDEF;
      level = "INFO";
      break;
    case ERROR:
      color = BRED;
      level = "ERROR";
      break;
    case WARN:
      color = BYELLOW;
      level = "WARN";
      break;
    case DEBUG:
      color = BBLACK;
      level = "DEBUG";
      break;
    default:
      color = KDEF;
      level = "INFO";
      break;
  }
  char buffer[256] = {0};
  char shell_output[512] = {0};
  char file_output[512] = {0};
  va_list _args;
  va_start(_args, fmt);
  vsnprintf(buffer, sizeof(buffer), fmt, _args);
  va_end(_args);
  if (!color)
    sprintf(shell_output, "%s  %s  %s\n" KNRM, timeInfoBuffer, level, buffer);
  else
    sprintf(shell_output, "%s%s  %s  %s\n" KNRM, color, timeInfoBuffer, level, buffer);
  sprintf(file_output, "%s  %s  %s\n", timeInfoBuffer, level, buffer);
  fprintf(stdout, "%s", shell_output);
  fflush(stdout);
  fprintf(fp, "%s", file_output);
  fflush(fp);
}
void csv_init() { csv_fp = fopen("./data/pos.csv", "a+"); }
void cvs_close() { fclose(csv_fp); }

void log_csv(const char* fmt, ...) {
  char buffer[1024] = {0};
  va_list arg;
  va_start(arg, fmt);
  // for (size_t i = 0; i < n; i++) {
  //   char* field = va_arg(arg, char*);
  //   strcat(buffer, field);
  //   if (i < n - 1) {
  //     strcat(buffer, ",");
  //   }
  // }
  vsnprintf(buffer, sizeof(buffer), fmt, arg);
  va_end(arg);
  // printf("Loggining into csv: %s\n", buffer);
  int count = fprintf(csv_fp, "%s\n", buffer);
  if (count < 0) {
    perror("Cannot write CSV file\n");
  }
}