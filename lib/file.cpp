#include <fstream>
#include <iso646.h>
#include <string>
#include <vector>

#include "rtl_exec.h"

namespace rtl {

std::string file$bytes(const std::string &filename)
{
    std::string r;
    std::ifstream f(filename, std::ios::binary);
    if (not f.is_open()) {
        throw RtlException("FileOpenError", filename);
    }
    for (;;) {
        char buf[16384];
        f.read(buf, sizeof(buf));
        std::streamsize n = f.gcount();
        if (n == 0) {
            break;
        }
        r.append(std::string(buf, n));
    }
    return r;
}

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
