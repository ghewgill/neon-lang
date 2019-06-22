#include <iso646.h>
#include <string.h>

#include <number.h>
#include "rtl_exec.h"

namespace rtl {

namespace ne_struct {

std::vector<unsigned char> packIEEE32(Number n)
{
    float x = number_to_float(n);
    std::vector<unsigned char> r(sizeof(x));
    memcpy(r.data(), &x, sizeof(x));
    return r;
}

std::vector<unsigned char> packIEEE64(Number n)
{
    double x = number_to_double(n);
    std::vector<unsigned char> r(sizeof(x));
    memcpy(r.data(), &x, sizeof(x));
    return r;
}

Number unpackIEEE32(const std::vector<unsigned char> &b)
{
    float x;
    memcpy(&x, b.data(), sizeof(x));
    return number_from_float(x);
}

Number unpackIEEE64(const std::vector<unsigned char> &b)
{
    double x;
    memcpy(&x, b.data(), sizeof(x));
    return number_from_double(x);
}

} // namespace ne_struct

} // namespace rtl
