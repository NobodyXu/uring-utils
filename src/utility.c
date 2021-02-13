#define _GNU_SOURCE /* For strerrordesc_np and O_NOFOLLOW */
#define _XOPEN_SOURCE 500 /* For snprintf */

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

int optiontosize(const char *str, size_t *dest, const char *option_name)
{
    const int base = (str[0] == '0' && str[1] == 'x') ? 16 : 10;

    const char *endptr;
    switch (strtosize(str, &endptr, base, dest)) {
        case -1:
            {
                char buffer[39];
                snprintf(buffer, sizeof(buffer),
                         "libc does not support parsing %d-based", base);

                eprintf("Error parsing %s: %s", option_name, buffer);
                return 0;
            }

        case -2:
            eprintf("Error parsing %s: %s",
                    option_name, "this option does not contain integer");
            return 0;

        case -3:
            eprintf("Error parsing %s: %s",
                    option_name, "the integer specified is out of range");
            return 0;

        case 0:
        default:
            break;
    }

    if (*endptr == '\0')
        return 1;

    static const char * const units[] = {
        "c",
        "w",
        "b",
        "kB",
        "K",
        "MB",
        "M",
        "xM",
        "GB",
        "G",
        "T",
        "P",
        "E",
        "Z",
        "Y",
    };
    static const size_t unit_cnt = sizeof(units) / sizeof(const char*);

    static const size_t KByte = 1024;
    static const size_t units_ratio[] = {
        1,
        2,
        512,
        1000,
        KByte,
        1000 * 1000,
        KByte * KByte,
        KByte * KByte,
        1000 * 1000 * 1000,
        KByte * KByte * KByte,
        KByte * KByte * KByte * KByte,
        KByte * KByte * KByte * KByte * KByte,
        KByte * KByte * KByte * KByte * KByte * KByte,
        KByte * KByte * KByte * KByte * KByte * KByte * KByte,
        KByte * KByte * KByte * KByte * KByte * KByte * KByte * KByte,
    };
    Static_assert(sizeof(units) / sizeof(const char*) == sizeof(units_ratio) / sizeof(size_t));

    for (size_t i = 0; i != unit_cnt; ++i) {
        if (strcmp(endptr, units[i]) == 0) {
            if (!checked_multiply(dest, units_ratio[i])) {
                eprintf("Error parsing %s: %s", option_name, "multiplcation overflowed");
                return 0;
            } else
                return 1;
        }
    }

    eprintf("Error parsing %s: %s", option_name, "unknown postfix");
    return 0;
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
