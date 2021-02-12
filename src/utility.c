#include <stdio.h>
#include <inttypes.h>
#include <errno.h>

#include "utility.h"

#define SIZE_T_MAX ((size_t) -1)

int strtosize(const char *str, const char* *endptr, int base, size_t *dest)
{
    errno = 0;
    uintmax_t result = strtoumax(str, (char* *) endptr, base);

    if (*endptr == str)
        return -2;

    if (errno == EINVAL)
        return -1;

    if (errno == ERANGE)
        return -3;

    if (result > SIZE_T_MAX)
        return -3;

    *dest = result;
    return 0;
}
