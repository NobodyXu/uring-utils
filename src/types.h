#ifndef  __uring_utils_types_H__
# define __uring_utils_types_H__

# include <stddef.h>

struct str_view {
    const char *str;
    size_t len;
};

static const struct str_view empty_str_view = {
    .str = (const char*) NULL,
    .len = 0
};

struct string {
    char *ptr;
    size_t len;
};

#endif
