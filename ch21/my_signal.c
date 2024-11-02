#include <signal.h>
#include <stddef.h>
#ifdef ABORT_WITH_FLUSH
#include <stdio.h>
#endif
#include <stdlib.h>

#include "my_signal.h"

__attribute__((noreturn)) void my_abort(void) {
#ifdef ABORT_WITH_FLUSH
  fcloseall();
#endif
  raise(SIGABRT);
  struct sigaction sigParam;
  sigParam.sa_handler = SIG_DFL;
  sigParam.sa_flags = 0;
  sigemptyset(&sigParam.sa_mask);
  sigaction(SIGABRT, &sigParam, NULL);
  raise(SIGABRT);
  _Exit(EXIT_FAILURE);
}
