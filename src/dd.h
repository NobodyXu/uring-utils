#ifndef  __uring_utils_dd_H__
# define __uring_utils_dd_H__

# include <stddef.h>

struct dd_param {
    size_t bs;
    /**
     * Set to -1 for dd inifinitely until EOF
     */
    size_t count;
    int in_fd;
    int out_fd;
};

int dd(struct io_uring *ring, const struct dd_param *param);

int dd_main(struct io_uring *ring, int argc, char* argv[]);

#endif
