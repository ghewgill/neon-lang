#include "intrinsic.h"

#include <assert.h>
#include <iso646.h>
#include <iostream>

#include <utf8.h>

#include "format.h"
#include "number.h"
#include "rtl_exec.h"

namespace rtl {

std::string global$chr(Number x)
{
    assert(number_is_integer(x));
    std::string r;
    utf8::append(number_to_uint32(x), std::back_inserter(r));
    return r;
}

std::string global$concat(const std::string &a, const std::string &b)
{
    return a + b;
}

Number global$int(Number a)
{
    return number_trunc(a);
}

std::string global$format(const std::string &str, const std::string &fmt)
{
    format::Spec spec;
    if (not format::parse(fmt, spec)) {
        throw RtlException(Exception_global$FormatException, fmt);
    }
    return format::format(str, spec);
}

Number global$max(Number a, Number b)
{
    if (number_is_greater(a, b)) {
        return a;
    } else {
        return b;
    }
}

Number global$min(Number a, Number b)
{
    if (number_is_less(a, b)) {
        return a;
    } else {
        return b;
    }
}

Number global$num(const std::string &s)
{
    return number_from_string(s);
}

Number global$ord(const std::string &s)
{
    if (utf8::distance(s.begin(), s.end()) != 1) {
        throw RtlException(Exception_global$ArrayIndexException, "ord() requires string of length 1");
    }
    auto it = s.begin();
    return number_from_uint32(utf8::next(it, s.end()));
}

std::string global$str(Number x)
{
    return number_to_string(x);
}

std::string global$strb(bool x)
{
    return x ? "TRUE" : "FALSE";
}

std::string global$substring(const std::string &s, Number offset, Number length)
{
    assert(number_is_integer(offset));
    assert(number_is_integer(length));
    auto start = s.begin();
    utf8::advance(start, number_to_uint32(offset), s.end());
    auto end = start;
    utf8::advance(end, number_to_uint32(length), s.end());
    return std::string(start, end);
}

} // namespace rtl
