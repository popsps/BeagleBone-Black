#ifndef BBSIGNAL_H
#define BBSIGNAL_H

#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define SIG_LOG_HEAD "[CS_695]"

sigset_t mask_all, mask_one, prev_one;
sigset_t mask_alls, mask_ones, prev_ones;
sigset_t custom_set;

void shell_write(char* s);

void registerSignals(void (*sig_handler)(int));
void register_sigpromask();
void block_signals();
void unblock_signals();

#endif
