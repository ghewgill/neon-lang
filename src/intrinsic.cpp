#include "intrinsic.h"

#include <assert.h>
#include <iso646.h>
#include <iostream>

#include <utf8.h>

#include "number.h"
#include "rtl_exec.h"

namespace rtl {

namespace global {

utf8string chr(Number x)
{
    if (not number_is_integer(x)) {
        throw RtlException(global::Exception_ValueRangeException, utf8string("chr() argument not an integer"));
    }
    if (number_is_negative(x) || number_is_greater(x, number_from_uint32(0x10ffff))) {
        throw RtlException(global::Exception_ValueRangeException, utf8string("chr() argument out of range 0-0x10ffff"));
    }
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

Number round(Number places, Number value)
{
    // TODO: Find out if there is a pow10 or equivalent function to compute scale.
    Number scale = number_from_uint32(1);
    for (int i = number_to_sint32(places); i > 0; i--) {
        scale = number_multiply(scale, number_from_uint32(10));
    }
    return number_divide(number_nearbyint(number_multiply(value, scale)), scale);
}

utf8string str(Number x)
{
    return utf8string(number_to_string(x));
}

} // namespace global

} // namespace rtl
