#include "number.h"
#include "rtl_platform.h"

namespace rtl {

Number time$now()
{
    return rtl_time_now();
}

} // namespace rtl
