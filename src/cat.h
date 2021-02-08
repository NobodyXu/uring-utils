#ifndef  __uring_utils_cat_H__
# define __uring_utils_cat_H__

# include <liburing.h>

/**
 * @param argv contains the argument to cat_main, but does not include path to the program
 */
int cat_main(struct io_uring *ring, int argc, char* argv[]);

#endif
