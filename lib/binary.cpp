#include <iso646.h>
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

template <> class traits<int64_t> {
public:
    static const unsigned int BITS = 64;
    static const Number MIN;
    static const Number MAX;
    static Number to_number(int64_t x) { return number_from_sint64(x); }
    static int64_t from_number(Number x) { return number_to_sint64(x); }
};

const Number traits<int64_t>::MIN = traits<int64_t>::to_number(std::numeric_limits<int64_t>::min());
const Number traits<int64_t>::MAX = traits<int64_t>::to_number(std::numeric_limits<int64_t>::max());

template <> class traits<uint64_t> {
public:
    static const unsigned int BITS = 64;
    static const Number MIN;
    static const Number MAX;
    static Number to_number(uint64_t x) { return number_from_uint64(x); }
    static uint64_t from_number(Number x) { return number_to_uint64(x); }
};

const Number traits<uint64_t>::MIN = traits<uint64_t>::to_number(std::numeric_limits<uint64_t>::min());
const Number traits<uint64_t>::MAX = traits<uint64_t>::to_number(std::numeric_limits<uint64_t>::max());

template <typename T> void range_check(Number x)
{
    if (number_is_less(x, traits<T>::MIN) || number_is_greater(x, traits<T>::MAX)) {
        throw RtlException(rtl::global::Exception_ValueRangeException, number_to_string(x));
    }
    if (not number_is_integer(x)) {
        throw RtlException(rtl::global::Exception_ValueRangeException, number_to_string(x));
    }
}

template <typename T> Number binary_and(Number x, Number y)
{
    range_check<T>(x);
    range_check<T>(y);
    return traits<T>::to_number(traits<T>::from_number(x) & traits<T>::from_number(y));
}

template <typename T> Number binary_extract(Number x, Number n, Number w)
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
    return traits<T>::to_number((traits<T>::from_number(x) >> b) & (static_cast<T>(1 << v) - 1));
}

template <typename T> bool binary_get(Number x, Number n)
{
    range_check<T>(x);
    range_check<uint32_t>(n);
    unsigned int b = number_to_uint32(n);
    if (b >= traits<T>::BITS) {
        return false;
    }
    return (traits<T>::from_number(x) & static_cast<T>(1 << b)) != 0;
}

template <typename T> Number binary_not(Number x)
{
    range_check<T>(x);
    return traits<T>::to_number(~traits<T>::from_number(x));
}

template <typename T> Number binary_or(Number x, Number y)
{
    range_check<T>(x);
    range_check<T>(y);
    return traits<T>::to_number(traits<T>::from_number(x) | traits<T>::from_number(y));
}

template <typename T> Number binary_replace(Number x, Number n, Number w, Number y)
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
    T mask = v < traits<T>::BITS ? static_cast<T>(1 << v) - 1 : ~0;
    return traits<T>::to_number((traits<T>::from_number(x) & ~(mask << b)) | (z << b));
}

template <typename T> Number binary_set(Number x, Number n, bool v)
{
    range_check<T>(x);
    range_check<uint32_t>(n);
    unsigned int b = number_to_uint32(n);
    if (b >= traits<T>::BITS) {
        return x;
    }
    if (v) {
        return traits<T>::to_number(traits<T>::from_number(x) | static_cast<T>(1 << b));
    } else {
        return traits<T>::to_number(traits<T>::from_number(x) & ~static_cast<T>(1 << b));
    }
}

template <typename T> Number binary_shift_left(Number x, Number n)
{
    range_check<T>(x);
    range_check<uint32_t>(n);
    unsigned int b = number_to_uint32(n);
    if (b >= traits<T>::BITS) {
        return traits<T>::to_number(0);
    }
    return traits<T>::to_number(traits<T>::from_number(x) << b);
}

template <typename T> Number binary_shift_right(Number x, Number n)
{
    range_check<T>(x);
    range_check<uint32_t>(n);
    unsigned int b = number_to_uint32(n);
    if (b >= traits<T>::BITS) {
        return traits<T>::to_number(0);
    }
    return traits<T>::to_number(traits<T>::from_number(x) >> b);
}

template <typename T> Number binary_shift_right_signed(Number x, Number n)
{
    range_check<T>(x);
    range_check<uint32_t>(n);
    unsigned int b = number_to_uint32(n);
    if (b >= traits<T>::BITS) {
        return traits<T>::to_number(0);
    }
    // TODO: use traits functions so shiftRightSigned64 will work properly.
    return number_from_sint32(number_to_sint32(x) >> b);
}

template <typename T> Number binary_xor(Number x, Number y)
{
    range_check<T>(x);
    range_check<T>(y);
    return traits<T>::to_number(traits<T>::from_number(x) ^ traits<T>::from_number(y));
}

namespace rtl {

namespace binary {

Number and32(Number x, Number y)                            { return binary_and<uint32_t>(x, y); }
Number and64(Number x, Number y)                            { return binary_and<uint64_t>(x, y); }
Number extract32(Number x, Number n, Number w)              { return binary_extract<uint32_t>(x, n, w); }
Number extract64(Number x, Number n, Number w)              { return binary_extract<uint64_t>(x, n, w); }
bool get32(Number x, Number n)                              { return binary_get<uint32_t>(x, n); }
bool get64(Number x, Number n)                              { return binary_get<uint64_t>(x, n); }
Number not32(Number x)                                      { return binary_not<uint32_t>(x); }
Number not64(Number x)                                      { return binary_not<uint64_t>(x); }
Number or32(Number x, Number y)                             { return binary_or<uint32_t>(x, y); }
Number or64(Number x, Number y)                             { return binary_or<uint64_t>(x, y); }
Number replace32(Number x, Number n, Number w, Number y)    { return binary_replace<uint32_t>(x, n, w, y); }
Number replace64(Number x, Number n, Number w, Number y)    { return binary_replace<uint64_t>(x, n, w, y); }
Number set32(Number x, Number n, bool v)                    { return binary_set<uint32_t>(x, n, v); }
Number set64(Number x, Number n, bool v)                    { return binary_set<uint64_t>(x, n, v); }
Number shiftLeft32(Number x, Number n)                      { return binary_shift_left<uint32_t>(x, n); }
Number shiftLeft64(Number x, Number n)                      { return binary_shift_left<uint64_t>(x, n); }
Number shiftRight32(Number x, Number n)                     { return binary_shift_right<uint32_t>(x, n); }
Number shiftRight64(Number x, Number n)                     { return binary_shift_right<uint64_t>(x, n); }
Number shiftRightSigned32(Number x, Number n)               { return binary_shift_right_signed<int32_t>(x, n); }
Number shiftRightSigned64(Number x, Number n)               { return binary_shift_right_signed<int64_t>(x, n); }
Number xor32(Number x, Number y)                            { return binary_xor<uint32_t>(x, y); }
Number xor64(Number x, Number y)                            { return binary_xor<uint64_t>(x, y); }

std::string xorBytes(const std::string &x, const std::string &y)
{
    std::string r;
    for (size_t i = 0; i < x.length(); i++) {
        r.push_back(x[i] ^ y[i]);
    }
    return r;
}

} // namespace binary

} // namespace rtl
