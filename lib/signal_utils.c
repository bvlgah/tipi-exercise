#include <assert.h>
#include <errno.h>
#include <stddef.h>
#include <string.h>

#include "signal_utils.h"
#include "str_utils.h"

void printSigset(FILE *stream, const sigset_t *sigs, unsigned indent) {
  for (unsigned int i = 1; i < _NSIG; ++i) {
    if (sigismember(sigs, i)) {
      for (unsigned j = 0; j < indent; ++j)
        putc(' ', stream);
      fprintf(stream, "%s\n", strsignal(i));
    }
  }
}

int parseSignal(const char *str, int *sig) {
  int value;
  if (parseInt(str, &value))
    return 1;
  if (value >= _NSIG) {
    errno = ERANGE;
    return 1;
  }
  *sig = value;
  return 0;
}

int my_siginterrupt(int sig, int interruptSyscall) {
  struct sigaction sigParam;

  if (sigaction(sig, NULL, &sigParam))
    return -1;

  if (interruptSyscall)
    sigParam.sa_flags &= ~SA_RESTART;
  else
    sigParam.sa_flags |= SA_RESTART;

  if (sigaction(sig, &sigParam, NULL))
    return -1;
  else
    return 0;
}
