#include <string.h>
#include <libgen.h>

#include <stdio.h>
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

int invoke(const char *function_name, int argc, char* argv[])
{
    if (strcmp(function_name, "cat") == 0)
        return cat_main(argc, argv);
    else {
        fprintf(stderr, "Unkown function: %s", function_name);
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
