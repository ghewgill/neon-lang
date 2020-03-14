#include <iostream>
#include <iso646.h>

#include "rtl_exec.h"

namespace rtl {

namespace ne_console {

utf8string input(const utf8string &prompt)
{
    std::cout << prompt.str();
    std::string r;
    if (not std::getline(std::cin, r)) {
        throw RtlException(ne_global::Exception_EndOfFileException, utf8string(""));
    }
    return utf8string(r);
}

} // namespace ne_console

} // namespace rtl
