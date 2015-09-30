#include <iso646.h>
#include <stdlib.h>
#include <string>
#include <vector>

#include "number.h"
#include "rtl_exec.h"
#include "utf8string.h"

static std::vector<utf8string> g_argv;

namespace rtl {

std::vector<utf8string> sys$argv()
{
    return g_argv;
}

void sys$exit(Number x)
{
    if (not number_is_integer(x)) {
        throw RtlException(Exception_InvalidValue, "sys.exit invalid parameter: " + number_to_string(x));
    }
    int r = number_to_sint32(x);
    if (r < 0 || r > 255) {
        throw RtlException(Exception_InvalidValue, "sys.exit invalid parameter: " + number_to_string(x));
    }
    exit(r);
}

} // namespace rtl

void rtl_sys_init(int argc, char *argv[])
{
    g_argv.clear();
    std::copy(argv, argv+argc, std::back_inserter(g_argv));
}
