#include <pthread.h>
#include <signal.h>
#include <stddef.h>

#include "sig_sysv.h"

static pthread_mutex_t sigsetLock = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;

sysv_sighandler_t sysv_sigset(int sig, sysv_sighandler_t newHandler) {
  sigset_t oldProcMask;
  sysv_sighandler_t oldHandler;
  struct sigaction oldSigAction;

  pthread_mutext_lock(&sigsetLock);

  if (newHandler == SYSV_SIG_HOLD) {
    sigset_t blocking;
    sigemptyset(&blocking);
    if (sigaddset(&blocking, sig))
      goto onError;
    if (sigprocmask(SIG_BLOCK, &blocking, &oldProcMask))
      goto onError;
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

    if (sigprocmask(SIG_BLOCK, NULL, &oldProcMask))
      goto onError;
    if (sigaction(sig, &newSigAction, &oldSigAction))
      goto onError;
  }

  if (sigismember(&oldProcMask, sig))
    oldHandler = SYSV_SIG_HOLD;
  else
    oldHandler = oldSigAction.sa_handler;

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

  if (sigprocmask(SIG_BLOCK, &blocking, NULL))
    return -1;
  else
    return 0;
}

int sysv_sigrelse(int sig) {
  sigset_t blocking;
  sigemptyset(&blocking);
  if (sigaddset(&blocking, sig))
    return -1;

  if (sigprocmask(SIG_UNBLOCK, &blocking, NULL))
    return -1;
  else
    return 0;
}

int sysv_sigignore(int sig) {
  struct sigaction newSigAction;
  newSigAction.sa_flags = 0;
  sigemptyset(&newSigAction.sa_mask);
  newSigAction.sa_handler = SIG_IGN;

  if (sigaction(sig, &newSigAction, NULL))
    return -1;
  else
    return 0;
}
