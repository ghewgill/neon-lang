#include <assert.h>
#include <iostream>

#include <utf8.h>

#include "cell.h"
#include "number.h"

namespace rtl {

namespace Array {

Number size(Cell *self)
{
    return number_from_uint64(self->array().size());
}

} // namespace Array

namespace String {

Number length(std::string *self)
{
    return number_from_uint64(self->length());
}

} // namespace String

Number abs(Number x)
{
    return number_abs(x);
}

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

std::string input(const std::string &prompt)
{
    std::cout << prompt;
    std::string r;
    std::getline(std::cin, r);
    return r;
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
    return number_from_string(s);
}

Number ord(const std::string &s)
{
    assert(utf8::distance(s.begin(), s.end()) == 1);
    auto it = s.begin();
    return number_from_uint32(utf8::next(it, s.end()));
}

void print(const std::string &s)
{
    std::cout << s << "\n";
}

std::string splice(const std::string &t, const std::string &s, Number offset, Number length)
{
    // TODO: utf8
    uint32_t o = number_to_uint32(offset);
    return s.substr(0, o) + t + s.substr(o + number_to_uint32(length));
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
    auto start = s.begin();
    utf8::advance(start, number_to_uint32(offset), s.end());
    auto end = start;
    utf8::advance(end, number_to_uint32(length), s.end());
    return std::string(start, end);
}

} // namespace rtl
