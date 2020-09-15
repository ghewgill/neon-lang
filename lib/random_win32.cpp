#include <vector>

#include "number.h"

namespace rtl {

namespace ne_random {

std::vector<unsigned char> bytes(Number count)
{
    size_t icount = number_to_uint32(count);
    std::vector<unsigned char> r(icount);
    // Get actual random bytes here.
    return r;
}

} // namespace ne_random

} // namespace rtl
