#include "file_utils.h"
#include <sys/stat.h>
#include <fcntl.h>

#include "error_report.h"

bool is_dir(const char *pathname) {
    struct stat file_stat;
    if (stat(pathname, &file_stat) == -1) {
        fatal("failed to get the stat of file");
    }

    return S_ISDIR(file_stat.st_mode);
}

mode_t get_mode(const char *pathname) {
    struct stat file_stat;
    if (stat(pathname, &file_stat) == -1) {
        fatal("failed to get the stat of file");
    }

    return file_stat.st_mode;
}
