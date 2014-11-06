#include "rtl_platform.h"

#include <time.h>
#include <windows.h>

const ULONGLONG FILETIME_UNIX_EPOCH = 11644473600000000ULL;

Number rtl_time_now()
{
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);
    ULARGE_INTEGER ticks;
    ticks.LowPart = ft.dwLowDateTime;
    ticks.HighPart = ft.dwHighDateTime;
    return number_divide(number_from_uint64(ticks.QuadPart - FILETIME_UNIX_EPOCH), number_from_uint32(10000000));
}
