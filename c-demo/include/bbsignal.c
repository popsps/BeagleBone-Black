#include "bbsignal.h"


void shell_write(char* s) {
  char output[255] = {0};
  sprintf(output, "\033[1;31m%s: %s\n\033[0m", SIG_LOG_HEAD, s);
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

/**
 * Registers signal required by Beaglebone programs
 **/
void register_sigpromask() {
  sigfillset(&mask_all);
  sigemptyset(&custom_set);
  sigaddset(&custom_set, SIGUSR1);
  sigaddset(&custom_set, SIGINT);
  sigaddset(&custom_set, SIGTSTP);
}

/**
 * block signals for the custom set of signals
 * block signnal using multithreaded compatible versioon of sigprocmask
 **/
void block_signals() { pthread_sigmask(SIG_BLOCK, &custom_set, NULL); }

/**
 * unblock signals for the custom set of signals
 **/
void unblock_signals() { pthread_sigmask(SIG_UNBLOCK, &custom_set, NULL); }

// void block_signals() { sigprocmask(SIG_BLOCK, &mask_one, &prev_one); }
// void unblock_signals() { sigprocmask(SIG_SETMASK, &prev_one, NULL); }