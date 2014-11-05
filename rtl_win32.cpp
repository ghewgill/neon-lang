#include "rtl_platform.h"

#include <time.h>
#include <windows.h>

Number rtl_time_now()
{
    SYSTEMTIME st;
    GetLocalTime(&st);
    struct tm tm;
    tm.tm_year = st.wYear - 1900;
    tm.tm_mon = st.wMonth - 1;
    tm.tm_mday = st.wDay;
    tm.tm_hour = st.wHour;
    tm.tm_min = st.wMinute;
    tm.tm_sec = st.wSecond;
    time_t now = mktime(&tm);
    return number_add(number_from_uint32(static_cast<uint32_t>(now)), number_divide(number_from_uint32(st.wMilliseconds), number_from_uint32(1000)));
}
