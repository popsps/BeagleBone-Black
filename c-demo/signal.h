#ifndef SIGNAL_H
#define SIGNAL_H

#define SIG_LOG_HEAD "[CS_695]"

void shell_write(char* s);
void registerSignals(void (*sig_handler)(int));

#endif
