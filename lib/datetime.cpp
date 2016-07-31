#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#define timegm _mkgmtime
#endif
#include <time.h>

#include "cell.h"

namespace rtl {

namespace datetime {

Cell gmtime(Number t)
{
    time_t x = number_to_sint32(t);
    struct tm *tm = ::gmtime(&x);
    Cell r;
    r.array_index_for_write(0) = Cell(number_from_uint32(tm->tm_sec));
    r.array_index_for_write(1) = Cell(number_from_uint32(tm->tm_min));
    r.array_index_for_write(2) = Cell(number_from_uint32(tm->tm_hour));
    r.array_index_for_write(3) = Cell(number_from_uint32(tm->tm_mday));
    r.array_index_for_write(4) = Cell(number_from_uint32(tm->tm_mon));
    r.array_index_for_write(5) = Cell(number_from_uint32(tm->tm_year));
    r.array_index_for_write(6) = Cell(number_from_uint32(tm->tm_wday));
    r.array_index_for_write(7) = Cell(number_from_uint32(tm->tm_yday));
    r.array_index_for_write(8) = Cell(number_from_uint32(tm->tm_isdst));
    r.array_index_for_write(10);
    return r;
}

Number timegm_(Cell &t)
{
    struct tm tm;
    tm.tm_sec  = number_to_uint32(t.array_index_for_read(0).number());
    tm.tm_min  = number_to_uint32(t.array_index_for_read(1).number());
    tm.tm_hour = number_to_uint32(t.array_index_for_read(2).number());
    tm.tm_mday = number_to_uint32(t.array_index_for_read(3).number());
    tm.tm_mon  = number_to_uint32(t.array_index_for_read(4).number());
    tm.tm_year = number_to_uint32(t.array_index_for_read(5).number());
    return number_from_uint64(::timegm(&tm));
}

} // namespace datetime

} // namespace rtl
