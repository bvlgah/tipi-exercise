// With this program, we may see how Linux kernel determines the delivery order
// of pending standard and real-time signal if both are present.
//
// 1. Run the program, such as
//
//    $ ./ch22/sig_order 60 &
//    [1] 763233
//
// 2. Send standard and real-time signals with kill(1) and signals/t_sigqueue
//    offered as the source code of TLPI respectively, like
//
//    $ ../tlpi-dist/signals/t_sigqueue 763233 56 100 4
//    ../tlpi-dist/signals/t_sigqueue: PID is 763249, UID is 1000
//    $ ../tlpi-dist/signals/t_sigqueue 763233 34 100 2
//    ../tlpi-dist/signals/t_sigqueue: PID is 763265, UID is 1000
//    $ kill -2 763233
//    $ kill -11 763233
//    $ kill -4 763233
//    signal Illegal instruction (4) received
//    signal Segmentation fault (11) received
//    signal Interrupt (2) received
//    real-time signal Real-time signal 0 (34) received, with si_value = 100
//    real-time signal Real-time signal 0 (34) received, with si_value = 101
//    real-time signal Real-time signal 22 (56) received, with si_value = 100
//    real-time signal Real-time signal 22 (56) received, with si_value = 101
//    real-time signal Real-time signal 22 (56) received, with si_value = 102
//    real-time signal Real-time signal 22 (56) received, with si_value = 103
//    exiting with EXIT_SUCCESS
//    [1]  + 763233 done       ./ch22/sig_order 60
//
// 3. From the above shell output, we can see that if both standard and
//    real-time signals are pending for a process on Linux 6.11.5:
//
//    - Signal delivery follows a consistent pattern.
//    - Standard signals are delivered before real-time signals. For multiple
//      pending standard signals, the delivery order is the reverse of the
//      sending order, so the last sent signal is handled first.
//    - The delivery of real-time signals conforms to the POSIX specification.

#include <signal.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <error_report.h>
#include <str_utils.h>

static void printUsage(const char *progName) {
  fprintf(stderr, "usage: %s [sleep seconds]\n", progName);
}

static void sigactionHandler(int sig, siginfo_t *info, void *ucontext) {
  if (info->si_code == SI_QUEUE)
    printf("real-time signal %s (%d) received, with si_value = %d\n",
        strsignal(sig), sig, info->si_value.sival_int);
  else
    printf("signal %s (%d) received\n", strsignal(sig), sig);
}

int main(int argc, const char *argv[]) {
  if (argc > 2) {
    printUsage(argv[0]);
    fatal("too many arguments");
  }

  unsigned sleepSeconds = 10;
  if (argc == 2 && parseUnsignedInt(argv[1], &sleepSeconds)) {
    printUsage(argv[0]);
    fatalVardic("unknown argument: %s", argv[1]);
  }

  sigset_t blocking;
  sigfillset(&blocking);
  if (sigprocmask(SIG_BLOCK, &blocking, NULL))
    fatalWithError("failed to block all signals");

  struct sigaction sigParam;
  sigParam.sa_flags = SA_SIGINFO;
  sigfillset(&sigParam.sa_mask);
  sigParam.sa_sigaction = &sigactionHandler;
  for (int sig = 1; sig < _NSIG; ++sig) {
    if (sigismember(&blocking, sig) &&
        (sig != SIGKILL && sig != SIGSTOP) &&
        sigaction(sig, &sigParam, NULL)) {
      printf("sig is %s (%d)\n", strsignal(sig), sig);
      fatalWithError("failed to install signal handler");
    }
  }
  // Give time for senders to send signals.
  unsigned remainingSeconds = sleepSeconds;
  while ((remainingSeconds = sleep(remainingSeconds)))
    continue;

  if (sigprocmask(SIG_UNBLOCK, &blocking, NULL))
    fatalWithError("failed to unblock all signals");

  // Give time for handling signals.
  remainingSeconds = sleepSeconds;
  while ((remainingSeconds = sleep(remainingSeconds)))
    continue;

  printf("exiting with EXIT_SUCCESS");
  return EXIT_SUCCESS;
}
