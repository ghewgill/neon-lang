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

std::vector<Number> file$data(const std::string &filename)
{
    std::vector<Number> r;
    std::ifstream f(filename, std::ios_base::binary);
    if (not f.is_open()) {
        throw RtlException("FileOpenError", filename);
    }
    f.seekg(0, f.end);
    size_t length = f.tellg();
    f.seekg(0, f.beg);
    for (size_t x = 0; x < length; x++) {
        uint8_t c;
        f.read(reinterpret_cast<char *>(&c), sizeof(c));
        r.push_back(number_from_uint8(c));
    }
    return r;
}

} // namespace rtl
