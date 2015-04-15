#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <stdlib.h>
#include <string>

#include "number.h"

namespace rtl {

std::string os$getenv(const std::string &name)
{
    return getenv(name.c_str());
}

Number os$system(const std::string &command)
{
    return number_from_sint32(system(command.c_str()));
}

}
