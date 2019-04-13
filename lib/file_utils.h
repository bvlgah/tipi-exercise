#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <stdbool.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

extern bool is_dir(const char *pathname);

mode_t get_mode(const char *pathname);

#ifdef __cplusplus
}
#endif

#endif
