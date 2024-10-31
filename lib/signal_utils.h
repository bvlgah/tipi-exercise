#ifndef SIGNAL_UTILS_H
#define SIGNAL_UTILS_H

#include <signal.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

extern void printSigset(FILE *stream, const sigset_t *sigs, unsigned indent);

extern int parseSignal(const char *str, int *sig);

extern int my_siginterrupt(int sig, int interruptSyscall);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // SIGNAL_UTILS_H
