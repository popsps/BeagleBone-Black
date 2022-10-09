#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
  time_t now;
  struct tm *timeInfo, *utcTimeInfo;
  time(&now);
  printf("now: %ld\n", now);
  time_t base = time(0);
  printf("base: %ld\n", base);

  timeInfo = localtime(&now);
  utcTimeInfo = gmtime(&now);
  char buff[20];
  printf("Current local time and date: %s\n", asctime(timeInfo));
  printf("Event occurred at (UTC): %s\n", asctime(utcTimeInfo));
  strftime(buff, sizeof(buff), "%Y-%m-%d %H:%M:%S", utcTimeInfo);
  printf("Event occurred at (UTC): %s\n", buff);

  // printf("Current local time and date: %s", now);
  // printf("Current local time and date: %s", timeInfo);

  time_t now2 = base;
  
  while (now2 - base < 5) now2 = time(0);
  printf("now2: %ld\n", now2);

  return 0;
}
