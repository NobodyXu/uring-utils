#ifndef  __uring_utils_utility_H__
# define __uring_utils_utility_H__

# include <stddef.h>
# include <sys/types.h>

# ifdef __cplusplus
#  define Static_assert(expr) static_assert((expr))
# else
#  define Static_assert(expr) _Static_assert((expr), # expr) 
# endif

/**
 * @return  0 on success,
 *         -1 if the given base is unsupported,
 *         -2 if str does not contain integer
 *         -3 if out of range
 */
int strtosize(const char *str, const char **endptr, int base, size_t *dest);

/**
 * @param filename the file must exist, or this function will fail.
 * @param flags can be either O_RDONLY, O_WRONLY or O_RDWR and or-ed with any one of the following:
 *               - O_TRUNC
 *               - O_NOFOLLOW
 *               - O_NOCTTY
 *               - O_APPEND
 * @return fd on success, -1 on error and print an err msg to stderr
 */
int openfile(const char *filename, int flags);

/**
 * @param filename the file will be created if not exist.
 * @param flags In addition to the ones specified in openfile, O_EXCL can also be specified.
 *
 * createfile would create a file if not exists.
 */
int createfile(const char *filename, int flags, mode_t mode);

#endif
