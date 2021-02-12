#include <string.h>

#include "utility.hpp"
#include "../src/parser.c"

template <size_t argc, size_t option_sz>
void test_parse_dd_like_option_impl(
    const char * const (&argv)[argc], const char * const (&options)[option_sz],
    const str_view (&expected_values)[option_sz], int expected_ret,
    const char *expected_output,
    int line, const char *file, 
    const char *msg = ""
) noexcept
{
    int ret;

    struct str_view values[option_sz];
    memset(values, 0, sizeof(values));

    struct string output = capture_stderr(
        [&]() noexcept
        {
            ret = parse_dd_like_option(argc, argv, options, option_sz, values);
        }
    );

    assert_eq_impl(expected_values, values, line, file, msg);
    assert_eq_impl(expected_ret, ret, line, file, msg);
    assert_eq_impl(expected_output, output.ptr, line, file, msg);

    free(output.ptr);
}

#define test_parse_dd_like_option(argv, options, \
                                  expected_values, expected_ret, expected_output, ...) \
    test_parse_dd_like_option_impl((argv), (options), \
                                   (expected_values), (expected_ret), (expected_output), \
                                   __LINE__, __FILE__, ## __VA_ARGS__)

int main(int argc, char* argv[])
{
    test_parse_dd_like_option(
        (const char* const[]){"invalid_arg"},
        (const char * const[]){"opt"},
        (struct str_view[]){empty_str_view},
        0,
        "Invalid argument \"invalid_arg\""
    );

    test_parse_dd_like_option(
        (const char* const[]){"invalid_opt="},
        (const char * const[]){"opt"},
        (struct str_view[]){empty_str_view},
        0,
        "Invalid option \"invalid_opt=\": Value is not provided"
    );

    test_parse_dd_like_option(
        (const char* const[]){"invalid_opt=a"},
        (const char * const[]){"opt"},
        (struct str_view[]){empty_str_view},
        0,
        "Invalid option \"invalid_opt=a\": Unknown option name"
    );

    test_parse_dd_like_option(
        (const char* const[]){"opt=a"},
        (const char * const[]){"opt"},
        ((str_view[]){
            str_view{"a", 1}
        }),
        1,
        ""
    );

    test_parse_dd_like_option(
        ((const char* const[]){"opt1=a", "opt2=b"}),
        ((const char * const[]){"opt1", "opt2"}),
        ((str_view[]){
            str_view{"a", 1},
            str_view{"b", 1}
        }),
        1,
        ""
    );

    test_parse_dd_like_option(
        ((const char* const[]){"opt1=a", "opt2=b", "opt3=2333"}),
        ((const char * const[]){"opt1", "opt2", "opt3"}),
        ((str_view[]){
            str_view{"a", 1},
            str_view{"b", 1},
            str_view{"2333", 4}
        }),
        1,
        ""
    );

    return 0;
}
