#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "error_report.h"

#define BUFFER_SIZE 300

extern int optind, optopt;

int main(int argc, char **argv) {
    bool append = false;
    char *optstr = ":a";
    int opt;
    while ((opt = getopt(argc, argv, optstr)) != -1) {
        switch (opt) {
            case 'a':
                append = true;
                break;
            case '?':
                fprintf(stderr, "unsupported option: %c", (char) optopt);
                fatal("");
        }
    }

    int flag = O_CREAT | O_WRONLY;
    if (append) {
        flag |= O_APPEND;
    } else {
        flag |= O_TRUNC;
    }
    mode_t mode = S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH;

    if (argc - optind > 1) {
        fatal("more than one output file specified");
    }
    char *pathname = argv[optind];

    int out_fd;
    if ((out_fd = open(pathname, flag, mode)) == -1) {
        fatal("unable to open the file");
    }

    char buffer[BUFFER_SIZE];
    ssize_t numRead, numWrite;
    while ((numRead = read(STDIN_FILENO, buffer, BUFFER_SIZE)) > 0) {
        numWrite = write(out_fd, buffer, numRead);
        if (numWrite == -1) {
            fatal("unable to write content to the file");
        } else if (numWrite < numRead) {
            fatal("unable to write the whole buffer");
        }
    }

    if (numRead == -1) {
        fatal("failed to read from standard input");
    }

    if (close(out_fd) == -1) {
        fatal("failed to close the output file");
    }

    return EXIT_SUCCESS;
}
