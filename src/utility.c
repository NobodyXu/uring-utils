#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
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
        fputs("O_RDONLY", stderr);
    else if (flags & O_WRONLY)
        fputs("O_WRONLY", stderr);
    else if (flags & O_RDWR)
        fputs("O_RDWR", stderr);

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
            fputs(flags_strs[i], stderr);
    }
}

int openfile(const char *filename, int flags)
{
    int fd = open_autorestart(filename, flags, 0);

    if (fd == -1) {
        fprintf(stderr, "Failed to %s %s using flags ", "open", filename);
        print_flags(flags);
        fprintf(stderr, " : %s\n", strerror(errno));
    }

    return fd;
}

int createfile(const char *filename, int flags, mode_t mode)
{
    int fd = open_autorestart(filename, flags | O_CREAT, mode);

    if (fd == -1) {
        fprintf(stderr, "Failed to %s %s using flags ", "create", filename);

        print_flags(flags);

        if (mode & S_ISUID)
            fputs(" with mode S_ISUID", stderr);
        if (mode & S_ISGID)
            fputs(" with mode S_ISGID", stderr);

        fprintf(stderr, " : %s\n", strerror(errno));
    }

    return fd;
}
