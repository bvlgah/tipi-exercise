//
// Here are the steps to test this program:
//
// 1. Run sig_ign with the number of seconds to sleep for and the signal number
//    to block, e.g.,
//
//    $ ./sig_ign 30 2 &
//    [1] 188862
//    all signals (except SIGKILL and SIGSTOP) have been blocked
//    ./sig_ign: PID is 188862, going to sleep for 30 seconds
//
// 2. Send the signal that has been blocked to the process like
//
//    $ kill -2 188862
//    current pending signals:
//        Interrupt
//    signal SIGINT has been ignored
//
//    [1]  + 188862 done       ./sig_ign 30 2
//
// 3. As a comparison, if you send SIGTERM (with the number being 15 on Linux
//    x86_64), you will see it is terminated, such as
//
//    current pending signals:
//        Interrupt
//        Terminated
//    signal SIGINT has been ignored
//
//    [1]  + 189478 terminated  ./sig_ign 30 2
//

#include <signal.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <error_report.h>
#include <signal_utils.h>
#include <str_utils.h>

#define SLEEP_SECONDS_DEFAULT   20

static void signalHandler(int sig) {
  printf("signal %s received\n", strsignal(sig));
}

static void printUsage(const char *progName) {
  fprintf(stderr, "Usage: %s seconds signum\n", progName);
}

int main(int argc, const char *argv[]) {
  if (argc != 3) {
    printUsage(argv[0]);
    fatal("wrong command-line arguments");
  }

  unsigned int sleepSeconds = SLEEP_SECONDS_DEFAULT;
  if (parseUnsignedInt(argv[1], &sleepSeconds))
    fatalWithError("failed to parse sleep time");

  int sig;
  if (parseSignal(argv[2], &sig))
    fatalWithError("failed to parse signal numer");

  sigset_t blockingSigs;
  sigfillset(&blockingSigs);
  if (sigprocmask(SIG_BLOCK, &blockingSigs, NULL))
    fatalWithError("failed to block signals");
  else
    puts("all signals (except SIGKILL and SIGSTOP) have been blocked");

  pid_t pid = getpid();
  printf("%s: PID is %ld, going to sleep for %d seconds \n",
      argv[0], (long) pid, sleepSeconds);
  while ((sleepSeconds = sleep(sleepSeconds)))
    continue;

  sigset_t pendingSigs;
  sigpending(&pendingSigs);
  fprintf(stdout, "\ncurrent pending signals:\n");
  printSigset(stdout, &pendingSigs, 4);

  struct sigaction sigParam;
  sigParam.sa_flags = 0;
  sigemptyset(&sigParam.sa_mask);
  sigParam.sa_handler = SIG_IGN;
  if (sigaction(sig, &sigParam, NULL))
    fatalWithError("failed to ignore signal SIGINT");
  puts("signal SIGINT has been ignored");

  sigset_t emptySigs;
  sigemptyset(&emptySigs);
  if (sigprocmask(SIG_SETMASK, &emptySigs, NULL))
    fatalWithError("failed to unblock signals");

  return EXIT_SUCCESS;
}
