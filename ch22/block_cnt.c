// With this program, we can see a process can resume execution from the stopped
// status even a signal handler for SIGCONT is established and SIGCONT is
// blocked. We can verify with the following steps:
//
// 1. Run the program and suspend it with CTRL+Z, such as
//
//    $ ./ch22/block_cnt 60
//    ^Z
//    [1]  + 759539 suspended  ./ch22/block_cnt 60
//
// 2. Send SIGTERM and SIGCONT to the process with kill(1), like
//
//    $ kill -SIGTERM 759539
//    $ kill -SIGCONT 759539
//    signal Terminated received
//    exiting...
//    [1]  + 759539 exit 0     ./ch22/block_cnt 60

#include <signal.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <error_report.h>
#include <str_utils.h>
#include "signal_utils.h"

void signalHandler(int sig) {
  printf("signal %s received\n", strsignal(sig));
}

void printUsage(const char *progName) {
  fprintf(stderr, "usage: %s [sleep seconds]\n", progName);
}

int main(int argc, const char *argv[]) {
  unsigned sleepSeconds = 10;
  if (argc > 2) {
    printUsage(argv[0]);
    fatal("too many arguments");
  }
  if (argc == 2 && parseUnsignedInt(argv[1], &sleepSeconds)) {
    printUsage(argv[0]);
    fatalVardic("unknown argument: %s", argv[1]);
  }

  struct sigaction sigParam;
  sigParam.sa_flags = 0;
  sigemptyset(&sigParam.sa_mask);
  sigParam.sa_handler = &signalHandler;
  if (sigaction(SIGTERM, &sigParam, NULL))
    fatalWithError("failed to install a handler for SIGTERM");
  if (sigaction(SIGCONT, &sigParam, NULL))
    fatalWithError("failed to install a handler for SIGCONT");

  sigset_t blocking;
  sigaddset(&blocking, SIGCONT);
  if (sigprocmask(SIG_BLOCK, &blocking, NULL))
    fatalWithError("failed to block signal");

  while ((sleepSeconds = sleep(sleepSeconds)))
    continue;

  puts("exiting...");
  exit(EXIT_SUCCESS);
}
