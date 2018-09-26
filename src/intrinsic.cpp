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

utf8string chr(Number x)
{
    assert(number_is_integer(x));
    std::string r;
    utf8::append(number_to_uint32(x), std::back_inserter(r));
    return utf8string(r);
}

utf8string concat(const utf8string &a, const utf8string &b)
{
    return a + b;
}

Number int_(Number a)
{
    return number_trunc(a);
}

utf8string format(const utf8string &str, const utf8string &fmt)
{
    format::Spec spec;
    if (not format::parse(fmt.str(), spec)) { // TODO: utf8 if necessary
        throw RtlException(global::Exception_FormatException, fmt);
    }
    return utf8string(format::format(str.str(), spec)); // TODO: utf8
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

Number num(const utf8string &s)
{
    Number n = number_from_string(s.str());
    if (number_is_nan(n)) {
        throw RtlException(global::Exception_ValueRangeException, utf8string("num() argument not a number"));
    }
    return n;
}

bool odd(Number x)
{
    if (not number_is_integer(x)) {
        throw RtlException(global::Exception_ValueRangeException, utf8string("odd() requires integer"));
    }
    return number_is_odd(x);
}

Number ord(const utf8string &ss)
{
    std::string s = ss.str(); // TODO
    if (utf8::distance(s.begin(), s.end()) != 1) {
        throw RtlException(global::Exception_ArrayIndexException, utf8string("ord() requires string of length 1"));
    }
    auto it = s.begin();
    return number_from_uint32(utf8::next(it, s.end()));
}

utf8string str(Number x)
{
    return utf8string(number_to_string(x));
}

utf8string strb(bool x)
{
    return utf8string(x ? "TRUE" : "FALSE");
}

utf8string substring(const utf8string &ss, Number offset, Number length)
{
    std::string s = ss.str(); // TODO
    assert(number_is_integer(offset));
    assert(number_is_integer(length));
    auto start = s.begin();
    try {
        utf8::advance(start, number_to_uint32(offset), s.end());
    } catch (utf8::not_enough_room) {
        throw RtlException(global::Exception_ArrayIndexException, utf8string("offset"));
    }
    auto end = start;
    try {
        utf8::advance(end, number_to_uint32(length), s.end());
    } catch (utf8::not_enough_room) {
        throw RtlException(global::Exception_ArrayIndexException, utf8string("length"));
    }
    return utf8string(std::string(start, end));
}

} // namespace global

} // namespace rtl
