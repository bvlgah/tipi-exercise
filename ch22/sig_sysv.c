#include <pthread.h>
#include <signal.h>
#include <stddef.h>

#include "sig_sysv.h"

typedef void (*posix_sighandler_t)(int);

static pthread_mutex_t sigsetLock = PTHREAD_MUTEX_INITIALIZER;

static sysv_sighandler_t convertSignalHandler(posix_sighandler_t handler) {
  if (handler == SIG_DFL)
    return SYSV_SIG_DFL;
  else if (handler == SIG_IGN)
    return SYSV_SIG_IGN;
  else
    return handler;
}

sysv_sighandler_t sysv_sigset(int sig, sysv_sighandler_t newHandler) {
  sigset_t newProcMask;
  sigset_t oldProcMask;
  sysv_sighandler_t oldHandler;
  struct sigaction oldSigAction;

  pthread_mutex_lock(&sigsetLock);

  sigemptyset(&newProcMask);
  if (sigaddset(&newProcMask, sig))
    goto onError;

  if (newHandler == SYSV_SIG_HOLD) {
    if (sigprocmask(SIG_BLOCK, &newProcMask, &oldProcMask))
      goto onError;
    if (sigismember(&oldProcMask, sig)) {
      // If `sig` is already being blocked.
      // We don't need to check whether `sigismember()` returns -1 when `sig` is
      // not a valid signal number, because the very first call to `sigaddset()`
      // has already performed this check.
      oldHandler = SYSV_SIG_HOLD;
      goto onSuccess;
    }
    if (sigaction(sig, NULL, &oldSigAction))
      goto onError;
  } else {
    struct sigaction newSigAction;
    newSigAction.sa_flags = 0;
    sigemptyset(&newSigAction.sa_mask);

    if (newHandler == SYSV_SIG_IGN)
      newSigAction.sa_handler = SIG_IGN;
    else if (newHandler == SYSV_SIG_DFL)
      newSigAction.sa_handler = SIG_DFL;
    else
      newSigAction.sa_handler = newHandler;

    if (sigprocmask(SIG_UNBLOCK, &newProcMask, &oldProcMask))
      goto onError;
    if (sigaction(sig, &newSigAction, &oldSigAction))
      goto onError;
    if (sigismember(&oldProcMask, sig)) {
      // If `sig` is already being blocked.
      // We don't need to check whether `sigismember()` returns -1 when `sig` is
      // not a valid signal number, because the very first call to `sigaddset()`
      // has already performed this check.
      oldHandler = SYSV_SIG_HOLD;
      goto onSuccess;
    }
  }

  oldHandler = convertSignalHandler(oldSigAction.sa_handler);

onSuccess:
  pthread_mutex_unlock(&sigsetLock);
  return oldHandler;

onError:
  pthread_mutex_unlock(&sigsetLock);
  return SYSV_SIG_ERROR;
}

int sysv_sighold(int sig) {
  sigset_t blocking;
  sigemptyset(&blocking);
  if (sigaddset(&blocking, sig))
    return -1;

  return sigprocmask(SIG_BLOCK, &blocking, NULL);
}

int sysv_sigrelse(int sig) {
  sigset_t blocking;
  sigemptyset(&blocking);
  if (sigaddset(&blocking, sig))
    return -1;

  return sigprocmask(SIG_UNBLOCK, &blocking, NULL);
}

int sysv_sigignore(int sig) {
  struct sigaction newSigAction;
  newSigAction.sa_flags = 0;
  sigemptyset(&newSigAction.sa_mask);
  newSigAction.sa_handler = SIG_IGN;

  return sigaction(sig, &newSigAction, NULL);
}
