#include "number.h"

#include <random>

static std::mt19937 g_gen;

namespace rtl {

Number random$uint32()
{
    return number_from_uint32(g_gen());
}

} // namespace rtl
