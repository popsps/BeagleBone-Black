#ifndef BBSIGNAL_H
#define BBSIGNAL_H

#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#define SIG_LOG_HEAD "[BB-SIGNAL]"

void register_signal_handler(void (*sig_handler)(int));
void register_sigpromask();
void block_signals();
void unblock_signals();

#endif
