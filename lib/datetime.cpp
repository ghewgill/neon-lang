#include <time.h>

#include "cell.h"

namespace rtl {

Cell datetime$gmtime(Number t)
{
    time_t x = number_to_sint32(t);
    struct tm *tm = gmtime(&x);
    Cell r;
    r.array().resize(11);
    r.array()[0] = Cell(number_from_uint32(tm->tm_sec));
    r.array()[1] = Cell(number_from_uint32(tm->tm_min));
    r.array()[2] = Cell(number_from_uint32(tm->tm_hour));
    r.array()[3] = Cell(number_from_uint32(tm->tm_mday));
    r.array()[4] = Cell(number_from_uint32(tm->tm_mon));
    r.array()[5] = Cell(number_from_uint32(tm->tm_year));
    r.array()[6] = Cell(number_from_uint32(tm->tm_wday));
    r.array()[7] = Cell(number_from_uint32(tm->tm_yday));
    r.array()[8] = Cell(number_from_uint32(tm->tm_isdst));
    r.array()[9] = Cell(tm->tm_zone);
    r.array()[10] = Cell(number_from_uint32(tm->tm_gmtoff));
    return r;
}

Number datetime$timegm(Cell &t)
{
    struct tm tm;
    tm.tm_sec  = number_to_uint32(t.array()[0].number());
    tm.tm_min  = number_to_uint32(t.array()[1].number());
    tm.tm_hour = number_to_uint32(t.array()[2].number());
    tm.tm_mday = number_to_uint32(t.array()[3].number());
    tm.tm_mon  = number_to_uint32(t.array()[4].number());
    tm.tm_year = number_to_uint32(t.array()[5].number());
    return number_from_uint64(timegm(&tm));
}

}
