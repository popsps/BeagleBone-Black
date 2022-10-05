#ifndef BBSIGNAL_H
#define BBSIGNAL_H

#define SIG_LOG_HEAD "[BB-SIGNAL]"

void shell_write(char* s);
void registerSignals(void (*sig_handler)(int));

#endif
