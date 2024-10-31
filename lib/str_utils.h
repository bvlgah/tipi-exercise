#ifndef STR_UTILS_H
#define STR_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

// The function returns 0 if it succeeds, otherwise it returns 1 and errno is
// set accordingly. errno is set to EINVAL if str is invalid for an unsigned
// integer, and ERANGE if str represents a negative integer or an unsigned
// integer larger than UINT_MAX.
extern int parseUnsignedInt(const char *str, unsigned int *result);

// The function returns 0 if it succeeds, otherwise it returns 1 and errno is
// set accordingly. errno is set to EINVAL if str is invalid for an integer,
// and ERANGE if str represents an integer that is less than INT_MIN or greater
// than INT_MAX.
extern int parseInt(const char *str, int *result);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // STR_UTILS_H
