#ifndef BBSIGNAL_H
#define BBSIGNAL_H

#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#define SIG_LOG_HEAD "[BB-SIGNAL]"

sigset_t mask_all, mask_one, prev_one;
sigset_t mask_alls, mask_ones, prev_ones;
sigset_t custom_set;

void register_signal_handler(void (*sig_handler)(int));
void register_sigpromask();
void block_signals();
void unblock_signals();

#endif
