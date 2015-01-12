#include <windows.h>

#include "number.h"

const ULONGLONG FILETIME_UNIX_EPOCH = 11644473600000000ULL;

namespace rtl {

Number time$now()
{
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);
    ULARGE_INTEGER ticks;
    ticks.LowPart = ft.dwLowDateTime;
    ticks.HighPart = ft.dwHighDateTime;
    return number_divide(number_from_uint64(ticks.QuadPart - FILETIME_UNIX_EPOCH), number_from_uint32(10000000));
}

} // namespace rtl
