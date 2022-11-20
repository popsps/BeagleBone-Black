#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
  time_t current_time;
  struct tm* utcTimeInfo;
  current_time = time(0);
  utcTimeInfo = gmtime(&current_time);
  char timeInfoBuffer[25] = {0};
  strftime(timeInfoBuffer, sizeof(timeInfoBuffer), "%Y-%m-%d %H:%M:%S", utcTimeInfo);
  fprintf(stdout, "%s - Running this application on boot\n", timeInfoBuffer);
  return 0;
}