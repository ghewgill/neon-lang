#include <iso646.h>
#include <string.h>

#include <number.h>
#include "rtl_exec.h"

namespace rtl {

namespace ne_struct {

std::vector<unsigned char> packIEEE32BE(Number n)
{
    float x = number_to_float(n);
    uint32_t i = *(uint32_t *)&x;
    std::vector<unsigned char> r(sizeof(x));
    r[0] = (i >> 24) & 0xff;
    r[1] = (i >> 16) & 0xff;
    r[2] = (i >>  8) & 0xff;
    r[3] = (i      ) & 0xff;
    return r;
}

std::vector<unsigned char> packIEEE32LE(Number n)
{
    float x = number_to_float(n);
    uint32_t i = *(uint32_t *)&x;
    std::vector<unsigned char> r(sizeof(x));
    r[3] = (i >> 24) & 0xff;
    r[2] = (i >> 16) & 0xff;
    r[1] = (i >>  8) & 0xff;
    r[0] = (i      ) & 0xff;
    return r;
}

std::vector<unsigned char> packIEEE64BE(Number n)
{
    double x = number_to_double(n);
    uint64_t i = *(uint64_t *)&x;
    std::vector<unsigned char> r(sizeof(x));
    r[0] = (i >> 56) & 0xff;
    r[1] = (i >> 48) & 0xff;
    r[2] = (i >> 40) & 0xff;
    r[3] = (i >> 32) & 0xff;
    r[4] = (i >> 24) & 0xff;
    r[5] = (i >> 16) & 0xff;
    r[6] = (i >>  8) & 0xff;
    r[7] = (i      ) & 0xff;
    return r;
}

std::vector<unsigned char> packIEEE64LE(Number n)
{
    double x = number_to_double(n);
    uint64_t i = *(uint64_t *)&x;
    std::vector<unsigned char> r(sizeof(x));
    r[7] = (i >> 56) & 0xff;
    r[6] = (i >> 48) & 0xff;
    r[5] = (i >> 40) & 0xff;
    r[4] = (i >> 32) & 0xff;
    r[3] = (i >> 24) & 0xff;
    r[2] = (i >> 16) & 0xff;
    r[1] = (i >>  8) & 0xff;
    r[0] = (i      ) & 0xff;
    return r;
}

Number unpackIEEE32BE(const std::vector<unsigned char> &b)
{
    uint32_t i = (b[0] << 24) |
                 (b[1] << 16) |
                 (b[2] <<  8) |
                 (b[3]      );
    float x = *(float *)&i;
    return number_from_float(x);
}

Number unpackIEEE32LE(const std::vector<unsigned char> &b)
{
    uint32_t i = (b[3] << 24) |
                 (b[2] << 16) |
                 (b[1] <<  8) |
                 (b[0]      );
    float x = *(float *)&i;
    return number_from_float(x);
}

Number unpackIEEE64BE(const std::vector<unsigned char> &b)
{
    uint64_t i = (static_cast<uint64_t>(b[0]) << 56) |
                 (static_cast<uint64_t>(b[1]) << 48) |
                 (static_cast<uint64_t>(b[2]) << 40) |
                 (static_cast<uint64_t>(b[3]) << 32) |
                 (static_cast<uint64_t>(b[4]) << 24) |
                 (static_cast<uint64_t>(b[5]) << 16) |
                 (static_cast<uint64_t>(b[6]) <<  8) |
                 (static_cast<uint64_t>(b[7])      );
    double x = *(double *)&i;
    return number_from_double(x);
}

Number unpackIEEE64LE(const std::vector<unsigned char> &b)
{
    uint64_t i = (static_cast<uint64_t>(b[7]) << 56) |
                 (static_cast<uint64_t>(b[6]) << 48) |
                 (static_cast<uint64_t>(b[5]) << 40) |
                 (static_cast<uint64_t>(b[4]) << 32) |
                 (static_cast<uint64_t>(b[3]) << 24) |
                 (static_cast<uint64_t>(b[2]) << 16) |
                 (static_cast<uint64_t>(b[1]) <<  8) |
                 (static_cast<uint64_t>(b[0])      );
    double x = *(double *)&i;
    return number_from_double(x);
}

} // namespace ne_struct

} // namespace rtl
