#include "number.h"

#include <random>

static std::random_device g_rd;
static std::mt19937 g_gen(g_rd());

namespace rtl {

namespace ne_random {

Number uint32()
{
    return number_from_uint32(g_gen());
}

} // namespace ne_random

} // namespace rtl
