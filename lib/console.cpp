#include <iostream>
#include <iso646.h>

#include "rtl_exec.h"

namespace rtl {

namespace ne_console {

bool input_internal(const utf8string &prompt, utf8string *line)
{
    std::cout << prompt.str();
    std::string r;
    if (not std::getline(std::cin, r)) {
        return false;
    }
    *line = utf8string(r);
    return true;
}

} // namespace ne_console

} // namespace rtl
