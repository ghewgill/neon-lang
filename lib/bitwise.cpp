#include <limits>

#include "number.h"
#include "rtl_exec.h"

template <typename T> class traits {};

template <> class traits<int32_t> {
public:
    static const unsigned int BITS = 32;
    static const Number MIN;
    static const Number MAX;
    static Number to_number(int32_t x) { return number_from_sint32(x); }
    static int32_t from_number(Number x) { return number_to_sint32(x); }
};

const Number traits<int32_t>::MIN = traits<int32_t>::to_number(std::numeric_limits<int32_t>::min());
const Number traits<int32_t>::MAX = traits<int32_t>::to_number(std::numeric_limits<int32_t>::max());

template <> class traits<uint32_t> {
public:
    static const unsigned int BITS = 32;
    static const Number MIN;
    static const Number MAX;
    static Number to_number(uint32_t x) { return number_from_uint32(x); }
    static uint32_t from_number(Number x) { return number_to_uint32(x); }
};

const Number traits<uint32_t>::MIN = traits<uint32_t>::to_number(std::numeric_limits<uint32_t>::min());
const Number traits<uint32_t>::MAX = traits<uint32_t>::to_number(std::numeric_limits<uint32_t>::max());

template <typename T> void range_check(Number x)
{
    if (number_is_less(x, traits<T>::MIN) || number_is_greater(x, traits<T>::MAX)) {
        throw RtlException("ValueRange", number_to_string(x));
    }
    if (not number_is_integer(x)) {
        throw RtlException("ValueNotInteger", number_to_string(x));
    }
}

template <typename T> Number bitwise_and(Number x, Number y)
{
    range_check<T>(x);
    range_check<T>(y);
    return traits<T>::to_number(traits<T>::from_number(x) & traits<T>::from_number(y));
}

template <typename T> Number bitwise_extract(Number x, Number n, Number w)
{
    range_check<T>(x);
    range_check<uint32_t>(n);
    range_check<uint32_t>(w);
    unsigned int b = number_to_uint32(n);
    if (b >= traits<T>::BITS) {
        return traits<T>::to_number(0);
    }
    unsigned int v = number_to_uint32(w);
    if (b + v > traits<T>::BITS) {
        v = traits<T>::BITS - b;
    }
    if (v == traits<T>::BITS) {
        return x;
    }
    return traits<T>::to_number((traits<T>::from_number(x) >> b) & ((1 << v) - 1));
}

template <typename T> bool bitwise_get(Number x, Number n)
{
    range_check<T>(x);
    range_check<uint32_t>(n);
    unsigned int b = number_to_uint32(n);
    if (b >= traits<T>::BITS) {
        return false;
    }
    return (traits<T>::from_number(x) & (1 << b)) != 0;
}

template <typename T> Number bitwise_not(Number x)
{
    range_check<T>(x);
    return traits<T>::to_number(~traits<T>::from_number(x));
}

template <typename T> Number bitwise_or(Number x, Number y)
{
    range_check<T>(x);
    range_check<T>(y);
    return traits<T>::to_number(traits<T>::from_number(x) | traits<T>::from_number(y));
}

template <typename T> Number bitwise_replace(Number x, Number n, Number w, Number y)
{
    range_check<T>(x);
    range_check<uint32_t>(n);
    range_check<uint32_t>(w);
    range_check<T>(y);
    unsigned int b = number_to_uint32(n);
    if (b >= traits<T>::BITS) {
        return traits<T>::to_number(0);
    }
    unsigned int v = number_to_uint32(w);
    if (b + v > traits<T>::BITS) {
        v = traits<T>::BITS - b;
    }
    T z = traits<T>::from_number(y);
    T mask = v < traits<T>::BITS ? (1 << v) - 1 : ~0;
    return traits<T>::to_number((traits<T>::from_number(x) & ~(mask << b)) | (z << b));
}

template <typename T> Number bitwise_set(Number x, Number n, bool v)
{
    range_check<T>(x);
    range_check<uint32_t>(n);
    unsigned int b = number_to_uint32(n);
    if (b >= traits<T>::BITS) {
        return x;
    }
    if (v) {
        return traits<T>::to_number(traits<T>::from_number(x) | (1 << b));
    } else {
        return traits<T>::to_number(traits<T>::from_number(x) & ~(1 << b));
    }
}

template <typename T> Number bitwise_shift_left(Number x, Number n)
{
    range_check<T>(x);
    range_check<uint32_t>(n);
    unsigned int b = number_to_uint32(n);
    if (b >= traits<T>::BITS) {
        return traits<T>::to_number(0);
    }
    return traits<T>::to_number(traits<T>::from_number(x) << b);
}

template <typename T> Number bitwise_shift_right(Number x, Number n)
{
    range_check<T>(x);
    range_check<uint32_t>(n);
    unsigned int b = number_to_uint32(n);
    if (b >= traits<T>::BITS) {
        return traits<T>::to_number(0);
    }
    return traits<T>::to_number(traits<T>::from_number(x) >> b);
}

template <typename T> Number bitwise_shift_right_signed(Number x, Number n)
{
    range_check<T>(x);
    range_check<uint32_t>(n);
    unsigned int b = number_to_uint32(n);
    if (b >= traits<T>::BITS) {
        return traits<T>::to_number(0);
    }
    return number_from_sint32(number_to_sint32(x) >> b);
}

template <typename T> Number bitwise_xor(Number x, Number y)
{
    range_check<T>(x);
    range_check<T>(y);
    return traits<T>::to_number(traits<T>::from_number(x) ^ traits<T>::from_number(y));
}

namespace rtl {

Number bitwise$and(Number x, Number y)
{
    return bitwise_and<uint32_t>(x, y);
}

Number bitwise$extract(Number x, Number n, Number w)
{
    return bitwise_extract<uint32_t>(x, n, w);
}

bool bitwise$get(Number x, Number n)
{
    return bitwise_get<uint32_t>(x, n);
}

Number bitwise$not(Number x)
{
    return bitwise_not<uint32_t>(x);
}

Number bitwise$or(Number x, Number y)
{
    return bitwise_or<uint32_t>(x, y);
}

Number bitwise$replace(Number x, Number n, Number w, Number y)
{
    return bitwise_replace<uint32_t>(x, n, w, y);
}

Number bitwise$set(Number x, Number n, bool v)
{
    return bitwise_set<uint32_t>(x, n, v);
}

Number bitwise$shift_left(Number x, Number n)
{
    return bitwise_shift_left<uint32_t>(x, n);
}

Number bitwise$shift_right(Number x, Number n)
{
    return bitwise_shift_right<uint32_t>(x, n);
}

Number bitwise$shift_right_signed(Number x, Number n)
{
    return bitwise_shift_right_signed<int32_t>(x, n);
}

Number bitwise$xor(Number x, Number y)
{
    return bitwise_xor<uint32_t>(x, y);
}

} // namespace rtl
