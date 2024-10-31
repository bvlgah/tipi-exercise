//
// This program is used to verify the behavior of the flag SA_NODEFER.
//
// If SA_NODEFER is specified, signal handling may be interrupted by the same
// signal, such as:
//
//     $ ./sig_nodefer 2
//     trying to raise Interrupt with flag SA_NODEFER
//     handling signal Interrupt the 1 times
//     handling signal Interrupt the 2 times
//     handling signal Interrupt the 3 times
//     it is about finishing handling signal Interrupt the 3 times
//     it is about finishing handling signal Interrupt the 2 times
//     it is about finishing handling signal Interrupt the 1 times
//
// Otherwise, the same signal is blocked during its handling:
//
//    $ ./sig_nodefer --defer 2
//    trying to raise Interrupt without flag SA_NODEFER
//    handling signal Interrupt the 1 times
//    it is about finishing handling signal Interrupt the 1 times
//    handling signal Interrupt the 2 times
//    it is about finishing handling signal Interrupt the 2 times
//    handling signal Interrupt the 3 times
//    it is about finishing handling signal Interrupt the 3 times
//

#include <signal.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <error_report.h>
#include <signal_utils.h>
#include <str_utils.h>

#define SIGNAL_REPEAT   3

static volatile sig_atomic_t signalCount;

static void signalHandler(int sig) {
  unsigned id = signalCount++;
  printf("handling signal %s the %u times\n",
      strsignal(sig), id + 1);
  if (signalCount < SIGNAL_REPEAT)
    raise(sig);
  printf("it is about finishing handling signal %s the %u times\n",
      strsignal(sig), id + 1);
}

static void printUsage(const char *progName) {
  fprintf(stderr, "Usage: %s [--defer] signum\n", progName);
}

int main(int argc, const char *argv[]) {
  int nodefer = 1;
  unsigned signumPos = 1;
  if (argc != 2 && argc != 3) {
    printUsage(argv[0]);
    fatal("no command-line arguments");
  }
  if (argc == 3) {
    if (strcmp(argv[1], "--defer")) {
      printUsage(argv[0]);
      fatal("unknown argument");
    } else {
      nodefer = 0;
    }
    signumPos = 2;
  }

  int sig;
  if (parseSignal(argv[signumPos], &sig))
    fatalWithError("failed to parse signal number");

  struct sigaction sigParam;
  sigemptyset(&sigParam.sa_mask);
  sigParam.sa_handler = &signalHandler;
  sigParam.sa_flags = nodefer ? SA_NODEFER : 0;

  if (sigaction(sig, &sigParam, NULL))
    fatalWithError("failed to establish signal handler");

  printf("trying to raise %s %s flag SA_NODEFER\n",
      strsignal(sig), nodefer ? "with" : "without");
  raise(sig);

  return EXIT_SUCCESS;
}
