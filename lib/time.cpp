#include <thread>

#include "number.h"
#include "rtl_platform.h"

namespace rtl {

namespace time {

void sleep(Number seconds)
{
    std::chrono::microseconds us(number_to_uint64(number_multiply(seconds, number_from_uint32(1000000))));
    std::this_thread::sleep_for(us);
}

} // namespace time

} // namespace rtl
