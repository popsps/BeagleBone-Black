#include "bbsignal.h"

sigset_t mask_all, mask_one, prev_one;
sigset_t mask_alls, mask_ones, prev_ones;
sigset_t custom_set;
/**
 * Register signal handler for SIGINT, SIGTSTP, and SIGUSR1
 **/
void register_signal_handler(void (*sig_handler)(int)) {
  struct sigaction new;
  new.sa_handler = sig_handler;
  sigaction(SIGINT, &new, NULL);
  sigaction(SIGTSTP, &new, NULL);
  sigaction(SIGUSR1, &new, NULL);
}

/**
 * Registers signal required by Beaglebone programs
 * Currently supporting SIGUSR1, SIGINT, SIGTSTP
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
 * block signal using multithreaded compatible version of sigprocmask
 **/
void block_signals() { pthread_sigmask(SIG_BLOCK, &custom_set, NULL); }

/**
 * unblock signals for the custom set of signals
 **/
void unblock_signals() { pthread_sigmask(SIG_UNBLOCK, &custom_set, NULL); }

// void block_signals() { sigprocmask(SIG_BLOCK, &mask_one, &prev_one); }
// void unblock_signals() { sigprocmask(SIG_SETMASK, &prev_one, NULL); }