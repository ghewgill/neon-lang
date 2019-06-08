#include <iso646.h>
#include <windows.h>

#include "number.h"

const ULONGLONG FILETIME_UNIX_EPOCH = 116444736000000000ULL;

namespace rtl {

namespace ne_time {

Number now()
{
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);
    ULARGE_INTEGER ticks;
    ticks.LowPart = ft.dwLowDateTime;
    ticks.HighPart = ft.dwHighDateTime;
    return number_divide(number_from_uint64(ticks.QuadPart - FILETIME_UNIX_EPOCH), number_from_uint32(10000000));
}

Number tick()
{
    static bool init = false;
    static Number frequency;
    if (not init) {
        init = true;
        LARGE_INTEGER freq;
        QueryPerformanceFrequency(&freq);
        frequency = number_from_uint64(freq.QuadPart);
    }
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    return number_divide(number_from_uint64(now.QuadPart), frequency);
}

} // namespace ne_time

} // namespace rtl
