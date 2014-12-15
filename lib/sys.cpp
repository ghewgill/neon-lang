#include <stdlib.h>
#include <string>
#include <vector>

#include "number.h"

static std::vector<std::string> g_argv;

namespace rtl {

std::vector<std::string> sys$argv()
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
