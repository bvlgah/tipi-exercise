// We can use this program to verify the assertion that sigwaitinfo() is faster
// than sigsuspend() for waiting on signals. It holds true in my environment
// settings. Here is the comparison:
//
// $ time ./ch22/sig_speed_sigwaitinfo -n 100000
// ./ch22/sig_speed_sigwaitinfo -n 100000  0.07s user 0.81s system 49% cpu 1.806 total
// $ time ../tlpi-dist/signals/sig_speed_sigsuspend 100000
// ../tlpi-dist/signals/sig_speed_sigsuspend 100000  0.08s user 1.15s system 49% cpu 2.480 total
//
// As far as I am concerned, with sigsuspend(), we have to install a signal
// handler that must run before a call to sigsuspend() returns. Additionally,
// the kernel must perform preparatory and clean-up work to run a signal
// handler. Therefore, at least there are two factors that contribute to the
// slow-down of using sigsuspend() compared with sigwaitinfo().

#include <errno.h>
#include <signal.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <error_report.h>
#include <str_utils.h>

static void doWork(pid_t pid, int sig, unsigned repeat, int isChild) {
  sigset_t blocking;
  const char *processName = isChild ? "child" : "parent";
  sigfillset(&blocking);
  if (sigprocmask(SIG_BLOCK, &blocking, NULL))
    fatalVardic("failed to block all signals in the %s process: %s",
        processName, strerror(errno));

  siginfo_t info;
  sigset_t unblocked;
  if (sigaddset(&unblocked, sig))
    fatalVardic("invalid signal number %d", sig);

  for (int i = 0; i < repeat; ++i) {
    if (isChild) {
      if (kill(pid, sig))
        fatalVardic(
            "failed to send signal %d to process %ld in the %s process: %s",
            sig, (long) pid, processName, strerror(sig));
      sigwaitinfo(&unblocked, &info);
    } else {
      sigwaitinfo(&unblocked, &info);
      if (kill(pid, sig))
        fatalVardic(
            "failed to send signal %d to process %ld in the %s process: %s",
            sig, (long) pid, processName, strerror(sig));
    }
  }
}

static void printUsage(const char *progName) {
  fprintf(stderr, "usage: %s -n repeat\n", progName);
}

int main(int argc, const char *argv[]) {
  unsigned repeat;
  if (argc != 3) {
    printUsage(argv[0]);
    fatal("wrong arguments");
  } else {
    if (strcmp(argv[1], "-n")) {
      printUsage(argv[0]);
      fatalVardic("unknown argument '%s'", argv[1]);
    }
    if (parseUnsignedInt(argv[2], &repeat)) {
      printUsage(argv[0]);
      fatalVardic("unknown argument '%s', a non-negative integer expected",
          argv[2]);
    }
  }

  const int sig = SIGUSR1;
  pid_t pid = fork();
  if (pid == -1)
    fatalWithError("failed to fork");
  else if (pid == 0)
    doWork(getppid(), sig, repeat, 1);
  else
    doWork(pid, sig, repeat, 0);

  return EXIT_SUCCESS;
}
