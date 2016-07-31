#include <fstream>
#include <iso646.h>
#include <string>
#include <vector>

#include "rtl_exec.h"

namespace rtl {

namespace file {

std::string readBytes(const std::string &filename)
{
    std::string r;
    std::ifstream f(filename, std::ios::binary);
    if (not f.is_open()) {
        throw RtlException(Exception_FileOpenException, filename);
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

std::vector<utf8string> readLines(const std::string &filename)
{
    std::vector<utf8string> r;
    std::ifstream f(filename);
    if (not f.is_open()) {
        throw RtlException(Exception_FileOpenException, filename);
    }
    std::string s;
    while (std::getline(f, s)) {
        r.push_back(s);
    }
    return r;
}

void writeBytes(const std::string &filename, const std::string &data)
{
    std::ofstream f(filename, std::ios::binary);
    if (not f.is_open()) {
        throw RtlException(Exception_FileOpenException, filename);
    }
    if (not f.write(data.c_str(), data.length())) {
        throw RtlException(Exception_FileWriteException, filename);
    }
}

void writeLines(const std::string &filename, const std::vector<utf8string> &lines)
{
    std::ofstream f(filename, std::ios::out | std::ios::trunc); // Truncate the file every time we open it to write lines to it.
    if (not f.is_open()) {
        throw RtlException(Exception_FileOpenException, filename);
    }
    for (auto s: lines) {
        f << s.str() << "\n";   // Write line, and line-ending for each element in the array.
        if (f.fail()) {
            // If the write fails for any reason, consider that a FileWriteError exception.
            throw RtlException(Exception_FileWriteException, filename);
        }
    }
}

} // namespace file

} // namespace rtl
