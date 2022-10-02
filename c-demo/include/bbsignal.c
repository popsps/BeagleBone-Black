#include "bbsignal.h"

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void shell_write(char* s) {
  char output[255] = {0};
  sprintf(output, "\033[1;31m%s%s\n\033[0m", SIG_LOG_HEAD, s);
  ssize_t res = write(STDOUT_FILENO, output, strlen(output));
  if (res < 0) {
    perror("Something broke while logging\n");
  }
}

void registerSignals(void (*sig_handler)(int)) {
  struct sigaction new;
  new.sa_handler = sig_handler;
  sigaction(SIGINT, &new, NULL);
  sigaction(SIGTSTP, &new, NULL);
  sigaction(SIGUSR1, &new, NULL);
}