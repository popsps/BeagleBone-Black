#ifndef BBSIGNAL_H
#define BBSIGNAL_H

#define SIG_LOG_HEAD "[CS_695]"

void shell_write(char* s);
void registerSignals(void (*sig_handler)(int));

#endif
