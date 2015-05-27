#include <stdlib.h>
#include <string>
#include <vector>

#include "number.h"
#include "utf8string.h"

static std::vector<utf8string> g_argv;

namespace rtl {

std::vector<utf8string> sys$argv()
{
    return g_argv;
}

void sys$exit(Number x)
{
    exit(number_to_sint32(x));
}

} // namespace rtl

void rtl_sys_init(int argc, char *argv[])
{
    g_argv.clear();
    std::copy(argv, argv+argc, std::back_inserter(g_argv));
}
