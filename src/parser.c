#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"

/**
 * @param key_arg should be convertible to const struct str_view*
 * @param x_arg   should be convertible to const char* (a.k.a. string)
 */
static int str_view_and_str_cmp(const void *key_arg, const void *x_arg)
{
    const struct str_view *key = (const struct str_view*) key_arg;
    const char * const * x = (const char* const *) x_arg;

    return strncmp(key->str, *x, key->len);
}

int parse_dd_like_option(int argc, const char* const argv[],
                         const char * const options[], size_t option_sz,
                         const char* values[])
{
    for (int i = 0; i != argc; ++i) {
        const char *arg = argv[i];

        const char *delimiter = strchr(arg, '=');
        if (delimiter == NULL) {
            fprintf(stderr, "Invalid argument \"%s\"", arg);
            return 0;
        } else if (*(delimiter + 1) == '\0') {
            fprintf(stderr, "Invalid option \"%s\": %s", arg, "Value is not provided");
            return 0;
        }

        const struct str_view option = {
            .str = arg,
            .len = (size_t) (delimiter - arg)
        };

        void *element = bsearch(&option,
                                options, option_sz, sizeof(const char*),
                                str_view_and_str_cmp);
        if (element) {
            values[(const char * const *) element - options] = delimiter + 1;
        } else {
            fprintf(stderr, "Invalid option \"%s\": %s", arg, "Unknown option name");
            return 0;
        }
    }

    return 1;
}
