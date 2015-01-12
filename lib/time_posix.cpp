#include <sys/time.h>

#include "number.h"

namespace rtl {

Number time$now()
{
    struct timeval tv;
    if (gettimeofday(&tv, NULL) != 0) {
        return number_from_uint32(0);
    }
    return number_add(number_from_uint32(tv.tv_sec), number_divide(number_from_uint32(tv.tv_usec), number_from_uint32(1e6)));
}

} // namespace rtl
