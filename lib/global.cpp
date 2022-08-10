#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <iso646.h>
#include <iostream>
#include <sstream>

#ifdef _MSC_VER
#ifndef snprintf
#define snprintf _snprintf
#endif
#endif

#include <utf8.h>

#include "cell.h"
#include "intrinsic.h"
#include "number.h"
#include "rtl_exec.h"

namespace rtl {

namespace ne_global {

void print(const std::shared_ptr<Object> &x)
{
    if (x == nullptr) {
        std::cout << "NIL\n";
        return;
    }
    std::cout << x->toString().str() << "\n";
}

} // namespace ne_global

} // namespace rtl
