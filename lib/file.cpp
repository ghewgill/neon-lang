#include <fstream>
#include <iso646.h>
#include <string>
#include <vector>

#include "rtl_exec.h"

namespace rtl {

std::vector<std::string> file$lines(const std::string &filename)
{
    std::vector<std::string> r;
    std::ifstream f(filename);
    if (not f.is_open()) {
	throw RtlException("FileOpenError", filename);
    }
    std::string s;
    while (std::getline(f, s)) {
	r.push_back(s);
    }
    return r;
}

} // namespace rtl
