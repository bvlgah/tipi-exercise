#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>

#include "str_utils.h"

static int parseLong(const char *str, long *result) {
  char *endPtr;
  errno = 0;
  long value = strtol(str, &endPtr, 10);
  if (errno)
    return 1;

  // If the remaining string is not of whitespace characters, it should be an
  // invalid string for an integer, e.g. "100 abc".
  while (*endPtr) {
    if (!isspace(*endPtr)) {
      errno = EINVAL;
      return 1;
    }
    ++endPtr;
  }

  *result = value;
  return 0;
}

int parseUnsignedInt(const char *str, unsigned int *result) {
  long value;

  if (parseLong(str, &value))
    return 1;
  if (value > UINT_MAX || value < 0) {
    errno = ERANGE;
    return 1;
  }
  // It is safe to do convertion since 0 <= value <= INT_MAX.
  *result = (unsigned int) value;
  return 0;
}

int parseInt(const char *str, int *result) {
  long value;

  if (parseLong(str, &value))
    return 1;
  if (value > INT_MAX || value < INT_MIN) {
    errno = ERANGE;
    return 1;
  }
  // It is safe to do convertion since INT_MIN <= value <= INT_MAX.
  *result = (int) value;
  return 0;
}
