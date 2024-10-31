#ifndef ERROR_REPORT_H
#define ERROR_REPORT_H

#ifdef __cplusplus
extern "C" {
#endif

extern void fatal(const char *message);

extern void fatalWithError(const char *message);

extern void fatalVardic(const char *restrict format, ...);

#ifdef __cplusplus
}
#endif

#endif
