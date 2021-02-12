#ifndef  __uring_utils_test_utility_H__
# define __uring_utils_test_utility_H__

# define _POSIX_C_SOURCE 200809L /* For open_memstream */

# include <stdio.h>
# include <err.h>
# include <string.h>

# include <utility>

# include "../src/types.h"

inline bool operator != (const str_view &x, const str_view &y) noexcept
{
    if (x.len != y.len)
        return true;
    else if (strncmp(x.str, y.str, x.len) != 0)
        return true;
    else
        return false;
}

template <class F, class ...Args>
inline struct string capture_stderr(F &&f, Args &&...args)
{
    struct string str = {
        .ptr = nullptr,
        .len = 0
    };

    FILE * const memstream = open_memstream(&str.ptr, &str.len);
    if (memstream == NULL)
        err(1, "open_memstream failed");

    FILE * const real_stderr = stderr;
    stderr = memstream;

    std::forward<F>(f)(std::forward<Args>(args)...);

    stderr = real_stderr;
    fclose(memstream);

    return str;
}

inline void assert_eq_impl_impl(int expected, int actual,
                                const char *expected_expr, const char *actual_expr,
                                int line, const char *func, const char *file, const char *msg)
{
    if (expected != actual)
        errx(1, "%s != %s on line %d, function %s of file %s failed: %s: "
                "expected = %d, actual = %d", 
                expected_expr, actual_expr, line, func, file, msg, expected, actual);
}

inline void assert_eq_impl_impl(const char *expected, const char *actual,
                                const char *expected_expr, const char *actual_expr,
                                int line, const char *func, const char *file, const char *msg)
{
    if (strcmp(expected, actual) != 0)
        errx(1, "%s != %s on line %d, function %s of file %s failed: %s: "
                "expected = %s, actual = %s", 
                expected_expr, actual_expr, line, func, file, msg, expected, actual);
}

template <size_t N>
inline void assert_eq_impl_impl(const str_view (&expected)[N], const str_view (&actual)[N],
                                const char *expected_expr, const char *actual_expr,
                                int line, const char *func, const char *file, const char *msg)
{
    for (size_t i = 0; i != N; ++i) {
        if (expected[i] != actual[i])
            errx(1, "%s[%zu] != %s[%zu] on line %d, function %s of file %s failed: %s: "
                    "expected = %.*s, actual = %.*s", 
                    expected_expr, i, actual_expr, i, line, func, file, msg,
                    static_cast<int>(expected[i].len), expected[i].str,
                    static_cast<int>(actual[i].len), actual[i].str);
    }
}

template <size_t N>
inline void assert_eq_impl_impl(const char * const (&expected)[N], const char * const (&actual)[N],
                                const char *expected_expr, const char *actual_expr,
                                int line, const char *func, const char *file, const char *msg)
{
    for (size_t i = 0; i != N; ++i) {
        if (expected[i] == actual[i])
            continue;
        if ( ((expected[i] == NULL) ^ (actual[i] == NULL)) || (strcmp(expected[i], actual[i]) != 0))
            errx(1, "%s[%zu] != %s[%zu] on line %d, function %s of file %s failed: %s: "
                    "expected = %s, actual = %s", 
                    expected_expr, i, actual_expr, i, line, func, file, msg,
                    expected[i], actual[i]);
    }
}

#define assert_eq_impl(expected, actual, line, file, msg)           \
    assert_eq_impl_impl((expected), (actual), # expected, # actual, \
                        (line), __PRETTY_FUNCTION__, (file), (msg))

#define assert_impl(expr, line, file, msg)                          \
    if (!(expr))                                                    \
        errx(1, "%s on line %d, function %s of file %s failed: %s", \
            # expr, (line), __PRETTY_FUNCTION__, (file), (msg))

#define assert(expr, msg) assert_impl((expr), __LINE__, __FILE__, (msg))

#endif
