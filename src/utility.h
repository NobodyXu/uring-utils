#ifndef  __uring_utils_utility_H__
# define __uring_utils_utility_H__

# include <stddef.h>

/**
 * @return  0 on success,
 *         -1 if the given base is unsupported,
 *         -2 if str does not contain integer
 *         -3 if out of range
 */
int strtosize(const char *str, const char **endptr, int base, size_t *dest);

#endif
