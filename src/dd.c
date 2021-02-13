/**
 * The goal of this program is to be compatible with dd.
 */

#include <liburing.h>
#include <stdio.h>
#include <string.h>

#include <sys/stat.h>
#include <fcntl.h>

#include "utility.h"
#include "parser.h"
#include "dd.h"
#include "dd_help.h"

int dd(struct io_uring *ring, const struct dd_param *param)
{
    return 0;
}

int dd_help()
{
    eputs(dd_help_str);
    return 0;
}

int dd_main(struct io_uring *ring, int argc, char* argv[])
{
    if (argc == 0)
        return dd_help();

    if (strcmp(argv[1], "--help") == 0)
        return dd_help();

    const char* values[4];

    if (!parse_dd_like_option(
            argc, (const char* const *) argv,
            (const char* const[]){
                "bs",
                "count",
                "if",
                "of",
            }, 1,
            values
        ))
        return 0;

    struct dd_param param = {
        .bs = 512,
        .count = -1,
        .in_fd = 0,
        .out_fd = 1
    };

    if (values[0] != NULL && !optiontosize(values[0], &param.bs, "bs"))
        return 0;
    if (values[1] != NULL && !optiontosize(values[1], &param.count, "count"))
        return 0;

    if (values[2] != NULL) {
        param.in_fd = openfile(values[2], O_RDONLY);
        if (param.in_fd == -1)
            return 0;
    }

    if (values[3] != NULL) {
        param.out_fd = createfile(values[3], O_WRONLY, S_IRUSR | S_IWUSR);
        if (param.out_fd == -1)
            return 0;
    }

    /* Use getsubopt for parsing conv */

    return dd(ring, &param);
}
