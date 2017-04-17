#include "intrinsic.h"

#include <assert.h>
#include <iso646.h>
#include <iostream>

#include <utf8.h>

#include "format.h"
#include "number.h"
#include "rtl_exec.h"

namespace rtl {

namespace global {

std::string chr(Number x)
{
    assert(number_is_integer(x));
    std::string r;
    utf8::append(number_to_uint32(x), std::back_inserter(r));
    return r;
}

std::string concat(const std::string &a, const std::string &b)
{
    return a + b;
}

Number int_(Number a)
{
    return number_trunc(a);
}

std::string format(const std::string &str, const std::string &fmt)
{
    format::Spec spec;
    if (not format::parse(fmt, spec)) {
        throw RtlException(global::Exception_FormatException, fmt);
    }
    return format::format(str, spec);
}

Number max(Number a, Number b)
{
    if (number_is_greater(a, b)) {
        return a;
    } else {
        return b;
    }
}

Number min(Number a, Number b)
{
    if (number_is_less(a, b)) {
        return a;
    } else {
        return b;
    }
}

Number num(const std::string &s)
{
    Number n = number_from_string(s);
    if (number_is_nan(n)) {
        throw RtlException(global::Exception_ValueRangeException, "num() argument not a number");
    }
    return n;
}

bool odd(Number x)
{
    if (not number_is_integer(x)) {
        throw RtlException(global::Exception_ValueRangeException, "odd() requires integer");
    }
    return number_is_odd(x);
}

Number ord(const std::string &s)
{
    if (utf8::distance(s.begin(), s.end()) != 1) {
        throw RtlException(global::Exception_ArrayIndexException, "ord() requires string of length 1");
    }
    auto it = s.begin();
    return number_from_uint32(utf8::next(it, s.end()));
}

std::string str(Number x)
{
    return number_to_string(x);
}

std::string strb(bool x)
{
    return x ? "TRUE" : "FALSE";
}

std::string substring(const std::string &s, Number offset, Number length)
{
    assert(number_is_integer(offset));
    assert(number_is_integer(length));
    if (number_to_uint32(offset) + number_to_uint32(length) > s.length()) {
        throw RtlException(Exception_global$StringIndex, "length is past the end of string");
    }
    auto start = s.begin();
    try {
        utf8::advance(start, number_to_uint32(offset), s.end());
    } catch (utf8::not_enough_room) {
        throw RtlException(global::Exception_ArrayIndexException, "offset");
    }
    auto end = start;
    try {
        utf8::advance(end, number_to_uint32(length), s.end());
    } catch (utf8::not_enough_room) {
        throw RtlException(global::Exception_ArrayIndexException, "length");
    }
    return std::string(start, end);
}

} // namespace global

} // namespace rtl
