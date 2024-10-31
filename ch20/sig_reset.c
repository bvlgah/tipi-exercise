#include <signal.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <error_report.h>
#include <signal_utils.h>

static void printUsage(const char *progName) {
  fprintf(stderr, "Usage: %s [--reset] signum\n", progName);
}

static void signalHandler(int sig) {
  printf("signal %s received\n", strsignal(sig));
}

int main(int argc, const char *argv[]) {
  int sig;
  int reset = 0;

  if (argc == 2) {
    if (parseSignal(argv[1], &sig)) {
      printUsage(argv[0]);
      fatalVardic("invalid signal number: %s", argv[1]);
    }
  } else if (argc == 3) {
    if (strcmp(argv[1], "--reset")) {
      printUsage(argv[0]);
      fatalVardic("unknown command-line option: %s", argv[1]);
    }
    if (parseSignal(argv[2], &sig)) {
      printUsage(argv[0]);
      fatalVardic("invalid signal number: %s", argv[2]);
    }
    reset = 1;
  } else {
    printUsage(argv[0]);
    fatalVardic("%s command-line arguments", argc == 1 ? "no" : "too many");
  }

  struct sigaction sigParam;
  sigemptyset(&sigParam.sa_mask);
  sigParam.sa_handler = &signalHandler;
  sigParam.sa_flags = reset ? SA_RESETHAND : 0;
  if (sigaction(sig, &sigParam, NULL))
    fatalWithError("failed to establish signal handler");

  if (raise(sig))
    fatalWithError("fail to trigger signal");
  if (raise(sig))
    fatalWithError("fail to trigger signal");

  return EXIT_SUCCESS;
}
