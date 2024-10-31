#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error_report.h"

void fatal(const char *message) {
    fprintf(stderr, "%s\n", message);
    exit(EXIT_FAILURE);
}

void fatalWithError(const char *message) {
  int error = errno;
  fprintf(stderr, "%s: %s\n", message, strerror(error));
  exit(EXIT_FAILURE);
}

void fatalVardic(const char *restrict format, ...) {
  va_list args;
  va_start(args, format);
  vfprintf(stderr, format, args);
  va_end(args);
  putc('\n', stderr);
  exit(EXIT_FAILURE);
}
