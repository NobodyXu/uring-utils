#ifndef  __uring_utils_parser_H__
# define __uring_utils_parser_H__

# include "types.h"

/**
 * @param options sorted array of options, should be of option_sz long
 * @param values The length of array should be of option_sz long and initialized to 0;
 *               The value of each entry is corresponding to the one in options;
 *               If the option is not specified, then str_view is set to empty_str_view (all 0).
 * @return 1 if succeeds, 0 if not.
 *
 * If failed to parse the args, then an error message will be printed to stderr before return.
 */
int parse_dd_like_option(int argc, const char* const argv[],
                         const char * const options[], size_t option_sz, 
                         struct str_view values[]);

#endif
