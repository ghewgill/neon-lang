#include <iso646.h>
#include <stdlib.h>
#include <string>
#include <vector>

#include "number.h"
#include "rtl_exec.h"
#include "utf8string.h"

namespace rtl {

Cell sys$args;

void sys$exit(Number x)
{
    if (not number_is_integer(x)) {
        throw RtlException(Exception_global$InvalidValueException, "sys.exit invalid parameter: " + number_to_string(x));
    }
    int r = number_to_sint32(x);
    if (r < 0 || r > 255) {
        throw RtlException(Exception_global$InvalidValueException, "sys.exit invalid parameter: " + number_to_string(x));
    }
    exit(r);
}

} // namespace rtl

void rtl_sys_init(int argc, char *argv[])
{
    std::vector<Cell> &a = rtl::sys$args.array_for_write();
    a.resize(argc);
    for (int i = 0; i < argc; i++) {
        a[i] = Cell(argv[i]);
    }
}
