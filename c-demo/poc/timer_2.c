#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
  struct timespec base, now;
  clock_gettime(CLOCK_MONOTONIC_RAW, &base);
  // do stuff
  while (1) {
    // sleep(2);
    usleep(10000);
    // now = time(0);
    // printf("now: %ld, base: %ld, now - base: %ld\n", now, base, now - base);
    clock_gettime(CLOCK_MONOTONIC_RAW, &now);
    uint64_t delta_us = (now.tv_sec - base.tv_sec) * 1000000 + (now.tv_nsec - base.tv_nsec) / 1000;
    printf("base [%ld, %ld]: \n", base.tv_sec, base.tv_nsec);
    printf("now [%ld, %ld]:\n", now.tv_sec, now.tv_nsec);
    printf("delta_us: %ld\n", delta_us);
  }

  return 0;
}
