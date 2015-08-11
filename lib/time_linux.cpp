#include <time.h>

#include "number.h"

namespace rtl {

static const Number NANOSECONDS_PER_SECOND = number_from_uint64(1000000000LL);

Number time$tick()
{
    timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return number_add(number_from_uint64(ts.tv_sec), number_divide(number_from_uint64(ts.tv_nsec), NANOSECONDS_PER_SECOND));
}

} // namespace rtl
