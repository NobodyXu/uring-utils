/* Include liburing.h first to make sure _XOPEN_SOURCE is set to 500 */
#include <liburing.h>

#include <stdio.h>
#include <string.h>
#include <libgen.h>

#include "cat.h"

int help()
{
    puts("uring-utils: multi-call binary that contains utilities writen using io_uring\n"
         "\n"
         "Usage: uring-utils [function [arguments]...]\n"
         "   or: uring-utils --list\n"
         "   or: function [arguments] (this would only work when the symlink is installed)");
    return 0;
}

int list()
{
    puts("cat");
    return 0;
}

int initialize_uring_and_call(
    int (*func)(struct io_uring *ring, int argc, char* argv[]),
    int argc,
    char* argv[]
)
{
    struct io_uring ring;
    int errno_v = io_uring_queue_init(16, &ring, 0);
    if (errno_v != 0) {
        fprintf(stderr, "io_uring_setup failed: %s\n", strerror(-errno_v));
        return 1;
    }

    int ret = func(&ring, argc, argv);

    io_uring_queue_exit(&ring);

    return ret;
}

int invoke(const char *function_name, int argc, char* argv[])
{
    if (strcmp(function_name, "cat") == 0)
        return initialize_uring_and_call(cat_main, argc, argv);
    else {
        fprintf(stderr, "Unkown function: %s\n", function_name);
        return 1;
    }
}

int main(int argc, char* argv[])
{
    const char *exe_name = basename(argv[0]);

    if (strcmp(exe_name, "uring-utils") != 0)
        return invoke(exe_name, argc - 1, argv + 1);

    if (argc == 1)
        return help();

    if (strcmp(argv[1], "--help") == 0)
        return help();
    else if (strcmp(argv[1], "--list") == 0)
        return list();
    else if (argv[1][0] == '-' && argv[1][1] == '-') {
        fprintf(stderr, "Unknown option: %s\n", argv[1]);
        return 1;
    } else
        return invoke(argv[1], argc - 2, argv + 2);
}
