#include <iso646.h>
#include <stdlib.h>
#include <string>
#include <vector>

#include "number.h"
#include "rtl_exec.h"
#include "utf8string.h"

namespace rtl {

namespace ne_sys {

Cell VAR_args;

void exit(Number x)
{
    if (not number_is_integer(x)) {
        throw PanicException(utf8string("sys.exit invalid parameter: " + number_to_string(x)));
    }
    int r = number_to_sint32(x);
    if (r < 0 || r > 255) {
        throw PanicException(utf8string("sys.exit invalid parameter: " + number_to_string(x)));
    }
    ::exit(r);
}

} // namespace ne_sys

} // namespace rtl

void rtl_sys_init(int argc, char *argv[])
{
    std::vector<Cell> &a = rtl::ne_sys::VAR_args.array_for_write();
    a.resize(argc);
    for (int i = 0; i < argc; i++) {
        a[i] = Cell(argv[i]);
    }
}
