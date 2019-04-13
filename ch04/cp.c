#define _GNU_SOURCE

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include "error_report.h"
#include "file_utils.h"

#define BUFFER_SIZE 300

#define MAX(n1, n2) (n1 > n2 ? n1 : n2)
#define MIN(n1, n2) (n1 < n2 ? n1 : n2)

static bool is_hole(int fd);
static off_t next_hole(int fd);
static off_t next_data(int fd);
static off_t get_position(int fd);
static void set_position(int fd, off_t position);

int main(int argc, char **argv) {
    if (argc != 3) {
        fatal("usage: ./cp SOURCE TARGET");
    }

    const char *src_path = argv[1];
    const char *dst_path = argv[2];
    if (is_dir(src_path)) {
        fatal("copying a directory is not supported currently");
    }

    mode_t mode = get_mode(src_path);
    int src_flags = O_RDONLY;
    int dst_flags = O_WRONLY | O_CREAT | O_TRUNC;
    int src_fd, dst_fd;
    if ((src_fd = open(src_path, src_flags)) == -1) {
        fatal("failed to open the source file");
    }
    if ((dst_fd = open(dst_path, dst_flags, mode)) == -1) {
        fatal("failed to open/create the target file");
    }

    char buffer[BUFFER_SIZE];
    while (true) {
        off_t start = next_data(src_fd);
        if (start == -1) {
            break;
        }
        set_position(src_fd, start);
        set_position(dst_fd, start);

        off_t end = next_hole(src_fd);
        for (off_t current = start; current < end;) {
            ssize_t numRead = read(src_fd, buffer, BUFFER_SIZE);
            if (numRead == -1) {
                fatal("failed to read the target file");
            }
            ssize_t numWrite = write(dst_fd, buffer, numRead);
            if (numWrite < numRead) {
                fatal("failed to write the whole buffer");
            }
            current += numRead;
        }
    }

    close(src_fd);
    close(dst_fd);
    return EXIT_SUCCESS;
}

off_t get_position(int fd) {
    off_t current = lseek(fd, 0, SEEK_CUR);
    if (current == -1) {
        fatal("failed to get the current position");
    }

    return current;
}

void set_position(int fd, off_t position) {
    if (lseek(fd, position, SEEK_SET) == -1) {
        fatal("failed to set the position");
    }
}

off_t next_hole(int fd) {
    off_t current = get_position(fd);
    off_t hole = lseek(fd, current, SEEK_HOLE);
    if (hole == -1) {
        fatal("failed to get the position of the next hole");
    }
    set_position(fd, current);

    return hole;
}

off_t next_data(int fd) {
    off_t current = get_position(fd);
    off_t data = lseek(fd, current, SEEK_DATA);
    if (data == -1 && errno != ENXIO) {
        fatal("failed to get the position of the next data");
    }
    set_position(fd, current);

    return data;
}

bool is_hole(int fd) {
    off_t current = get_position(fd);
    off_t hole = next_hole(fd);

    return current == hole;
}
