#ifndef SIG_SYSV_H
#define SIG_SYSV_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef void (*sysv_sighandler_t)(int);

// This is the default disposition for a signal, and it is signal-dependent.
#define SYSV_SIG_DFL                      ((sysv_sighandler_t) -2)
// This is used to ignore a signal, except for `SIGKILL` and `SIGSTOP`, which
// cannot be ignored on Linux.
#define SYSV_SIG_IGN                      ((sysv_sighandler_t) -3)
// This is used to block a signal by adding it to a process's signal mask, while
// keeping the signal's disposition unchanged.
#define SYSV_SIG_HOLD                     ((sysv_sighandler_t) -4)
// This is the error result returned by `sysv_sigset()`.
#define SYSV_SIG_ERROR                    ((sysv_sighandler_t) -1)

// This function reimplements the System V signal API sigset(), providing
// reliable semantics as follows:
//
// 1. While a signal handler is running, it cannot be interrupted by the same
//    signal.
// 2. The disposition for a signal remains the same during and after execution.
extern sysv_sighandler_t sysv_sigset(int sig, sysv_sighandler_t newHandler);

// This function reimplements the System V signal API `sighold()`, which adds
// `sig` to the calling process's signal mask.
extern int sysv_sighold(int sig);

// This function reimplements the System V signal API `sigrelse()`, which
// removes `sig` from the calling process's signal mask.
extern int sysv_sigrelse(int sig);

// This function reimplements the System V signal API `sigignore()`, which sets
// the calling process's disposition for `sig` to `SYSV_SIG_IGN`.
extern int sysv_sigignore(int sig);

// This function reimplements the System V signal API `sigpause()`, which
// temporarily removes `sig` from the calling process's signal mask and waits
// for signal until the function returns.
extern int sysv_sigpause(int sig);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // SIG_SYSV_H
