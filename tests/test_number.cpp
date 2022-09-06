#include <assert.h>
#include <limits>

#include "number.h"

void do_verify_eq(int line, std::string source, std::string value, std::string expected)
{
    if (value == expected) {
        return;
    }
    fprintf(stderr, "    line: %d\n", line);
    fprintf(stderr, "  source: %s\n", source.c_str());
    fprintf(stderr, "   value: %s\n", value.c_str());
    fprintf(stderr, "expected: %s\n", expected.c_str());
    assert(value == expected);
}

#define verify_eq(value, expected) do_verify_eq(__LINE__, #value, value, expected)

int main()
{
    verify_eq(number_to_string(number_from_uint64(0)), "0");
    verify_eq(number_to_string(number_from_uint64(1)), "1");
    verify_eq(number_to_string(number_from_uint64(std::numeric_limits<uint32_t>::max())), "4294967295");
    verify_eq(number_to_string(number_from_uint64(static_cast<int64_t>(std::numeric_limits<uint32_t>::max())+1)), "4294967296");
    verify_eq(number_to_string(number_from_uint64(std::numeric_limits<int64_t>::max())), "9223372036854775807");
    verify_eq(number_to_string(number_from_uint64(std::numeric_limits<uint64_t>::max())), "18446744073709551615");

    verify_eq(number_to_string(number_from_sint64(0)), "0");
    verify_eq(number_to_string(number_from_sint64(1)), "1");
    verify_eq(number_to_string(number_from_sint64(std::numeric_limits<uint32_t>::max())), "4294967295");
    verify_eq(number_to_string(number_from_sint64(static_cast<int64_t>(std::numeric_limits<uint32_t>::max())+1)), "4294967296");
    verify_eq(number_to_string(number_from_sint64(std::numeric_limits<int64_t>::max())), "9223372036854775807");
    verify_eq(number_to_string(number_from_sint64(-1)), "-1");
    verify_eq(number_to_string(number_from_sint64(-4294967295LL)), "-4294967295");
    verify_eq(number_to_string(number_from_sint64(-4294967296LL)), "-4294967296");
    verify_eq(number_to_string(number_from_sint64(-9223372036854775807LL)), "-9223372036854775807");
    verify_eq(number_to_string(number_from_sint64(std::numeric_limits<int64_t>::min())), "-9223372036854775808");

    verify_eq(std::to_string(number_to_uint64(number_from_string("0"))), "0");
    verify_eq(std::to_string(number_to_uint64(number_from_string("1"))), "1");
    verify_eq(std::to_string(number_to_uint64(number_from_string("4294967295"))), "4294967295");
    verify_eq(std::to_string(number_to_uint64(number_from_string("4294967296"))), "4294967296");
    verify_eq(std::to_string(number_to_uint64(number_from_string("9223372036854775807"))), "9223372036854775807");
    verify_eq(std::to_string(number_to_uint64(number_from_string("18446744073709551615"))), "18446744073709551615");

    verify_eq(std::to_string(number_to_sint64(number_from_string("0"))), "0");
    verify_eq(std::to_string(number_to_sint64(number_from_string("1"))), "1");
    verify_eq(std::to_string(number_to_sint64(number_from_string("4294967295"))), "4294967295");
    verify_eq(std::to_string(number_to_sint64(number_from_string("4294967296"))), "4294967296");
    verify_eq(std::to_string(number_to_sint64(number_from_string("9223372036854775807"))), "9223372036854775807");
    verify_eq(std::to_string(number_to_sint64(number_from_string("-1"))), "-1");
    verify_eq(std::to_string(number_to_sint64(number_from_string("-4294967295"))), "-4294967295");
    verify_eq(std::to_string(number_to_sint64(number_from_string("-4294967296"))), "-4294967296");
    verify_eq(std::to_string(number_to_sint64(number_from_string("-9223372036854775807"))), "-9223372036854775807");
    verify_eq(std::to_string(number_to_sint64(number_from_string("-9223372036854775808"))), "-9223372036854775808");
}
