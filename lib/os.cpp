#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <ctype.h>
#include <iso646.h>
#include <stdlib.h>
#include <string>

#include "number.h"
#include "utf8string.h"

namespace rtl {

namespace ne_os {

utf8string getenv(const utf8string &name)
{
    const char *r = ::getenv(name.c_str());
    if (r == NULL) {
        return utf8string();
    }
    return utf8string(r);
}

} // namespace ne_os

} // namespace rtl
