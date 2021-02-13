#define _POSIX_C_SOURCE 200809L
#define _GNU_SOURCE

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <inttypes.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

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

int checked_multiply(size_t *dest, size_t ratio)
{
    const size_t val = *dest * ratio;
    if (val < *dest)
        return 0;

    *dest = val;
    return 1;
}

void eputs(const char *s)
{
    fputs(s, stderr);
}

void eprintf(const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
}

const char* strerror_mtsafe(int errnum)
{
    return strerrordesc_np(errnum);
}

const char *strerrno()
{
    return strerror_mtsafe(errno);
}

int open_autorestart(const char *filename, int flags, mode_t mode)
{
    int fd;

    do {
        fd = open(filename, flags, mode);
    } while (fd == -1 && errno == EINTR);

    return fd;
}

void print_flags(int flags)
{
    if (flags & O_RDONLY)
        eputs("O_RDONLY");
    else if (flags & O_WRONLY)
        eputs("O_WRONLY");
    else if (flags & O_RDWR)
        eputs("O_RDWR");

    static const int flags_arr[] = {
        O_TRUNC,
        O_NOFOLLOW,
        O_NOCTTY,
        O_APPEND,
        O_EXCL,
    };
    static const size_t flags_cnt = sizeof(flags_arr) / sizeof(int);
    static const char* const flags_strs[] = {
        ",O_TRUNC",
        ",O_NOFOLLOW",
        ",O_NOCTTY",
        ",O_APPEND",
        ",O_EXCL",
    };

    Static_assert((sizeof(flags_arr) / sizeof(int)) == (sizeof(flags_strs) / sizeof(const char*)));

    for (size_t i = 0; i != flags_cnt; ++i) {
        if (flags & flags_arr[i])
            eputs(flags_strs[i]);
    }
}

int openfile(const char *filename, int flags)
{
    int fd = open_autorestart(filename, flags, 0);

    if (fd == -1) {
        eprintf("Failed to %s %s using flags ", "open", filename);
        print_flags(flags);
        eprintf(" : %s\n", strerrno());
    }

    return fd;
}

int createfile(const char *filename, int flags, mode_t mode)
{
    int fd = open_autorestart(filename, flags | O_CREAT, mode);

    if (fd == -1) {
        eprintf("Failed to %s %s using flags ", "create", filename);

        print_flags(flags);

        if (mode & S_ISUID)
            eputs(" with mode S_ISUID");
        if (mode & S_ISGID)
            eputs(" with mode S_ISGID");

        eprintf(" : %s\n", strerrno());
    }

    return fd;
}
