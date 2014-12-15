#include <thread>

#include "number.h"
#include "rtl_platform.h"

namespace rtl {

Number time$now()
{
    return rtl_time_now();
}

void time$sleep(Number seconds)
{
    std::chrono::microseconds us(number_to_uint64(number_multiply(seconds, number_from_uint32(1000000))));
    std::this_thread::sleep_for(us);
}

} // namespace rtl
