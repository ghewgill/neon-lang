#include "number.h"

namespace rtl {

Number bitwise$and(Number x, Number y)
{
    return number_from_uint32(number_to_uint32(x) & number_to_uint32(y));
}

Number bitwise$extract(Number x, Number n, Number w)
{
    uint32_t b = number_to_uint32(n);
    if (b >= 32) {
        return number_from_uint32(0);
    }
    uint32_t v = number_to_uint32(w);
    if (b + v > 32) {
        v = 32 - b;
    }
    if (v == 32) {
        return x;
    }
    return number_from_uint32((number_to_uint32(x) >> b) & ((1 << v) - 1));
}

bool bitwise$get(Number x, Number n)
{
    uint32_t b = number_to_uint32(n);
    if (b >= 32) {
        return false;
    }
    return (number_to_uint32(x) & (1 << b)) != 0;
}

Number bitwise$not(Number x)
{
    return number_from_uint32(~number_to_uint32(x));
}

Number bitwise$or(Number x, Number y)
{
    return number_from_uint32(number_to_uint32(x) | number_to_uint32(y));
}

Number bitwise$replace(Number x, Number n, Number w, Number y)
{
    uint32_t b = number_to_uint32(n);
    if (b >= 32) {
        return number_from_uint32(0);
    }
    uint32_t v = number_to_uint32(w);
    if (b + v > 32) {
        v = 32 - b;
    }
    uint32_t z = number_to_uint32(y);
    uint32_t mask = v < 32 ? (1 << v) - 1 : ~0;
    return number_from_uint32((number_to_uint32(x) & ~(mask << b)) | (z << b));
}

Number bitwise$set(Number x, Number n, bool v)
{
    uint32_t b = number_to_uint32(n);
    if (b >= 32) {
        return x;
    }
    if (v) {
        return number_from_uint32(number_to_uint32(x) | (1 << b));
    } else {
        return number_from_uint32(number_to_uint32(x) & ~(1 << b));
    }
}

Number bitwise$shift_left(Number x, Number n)
{
    uint32_t b = number_to_uint32(n);
    if (b >= 32) {
        return number_from_uint32(0);
    }
    return number_from_uint32(number_to_uint32(x) << b);
}

Number bitwise$shift_right(Number x, Number n)
{
    uint32_t b = number_to_uint32(n);
    if (b >= 32) {
        return number_from_uint32(0);
    }
    return number_from_uint32(number_to_uint32(x) >> b);
}

Number bitwise$xor(Number x, Number y)
{
    return number_from_uint32(number_to_uint32(x) ^ number_to_uint32(y));
}

} // namespace rtl
