#include <stdio.h>
#include <stdlib.h>

#include "error_report.h"

void fatal(const char *message) {
    fprintf(stderr, "%s\n", message);
    exit(EXIT_FAILURE);
}
