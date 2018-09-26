#include <fstream>
#include <iso646.h>
#include <string>
#include <vector>

#include "rtl_exec.h"

namespace rtl {

namespace file {

std::vector<unsigned char> readBytes(const utf8string &filename)
{
    std::vector<unsigned char> r;
    std::ifstream f(filename.str(), std::ios::binary);
    if (not f.is_open()) {
        throw RtlException(Exception_FileException_Open, filename);
    }
    for (;;) {
        char buf[16384];
        f.read(buf, sizeof(buf));
        std::streamsize n = f.gcount();
        if (n == 0) {
            break;
        }
        std::copy(buf, buf+n, std::back_inserter(r));
    }
    return r;
}

std::vector<utf8string> readLines(const utf8string &filename)
{
    std::vector<utf8string> r;
    std::ifstream f(filename.str());
    if (not f.is_open()) {
        throw RtlException(Exception_FileException_Open, filename);
    }
    std::string s;
    while (std::getline(f, s)) {
        r.push_back(utf8string(s));
    }
    return r;
}

void writeBytes(const utf8string &filename, const std::vector<unsigned char> &data)
{
    std::ofstream f(filename.str(), std::ios::binary);
    if (not f.is_open()) {
        throw RtlException(Exception_FileException_Open, filename);
    }
    if (not f.write(reinterpret_cast<const char *>(data.data()), data.size())) {
        throw RtlException(Exception_FileException_Write, filename);
    }
}

void writeLines(const utf8string &filename, const std::vector<utf8string> &lines)
{
    std::ofstream f(filename.str(), std::ios::out | std::ios::trunc); // Truncate the file every time we open it to write lines to it.
    if (not f.is_open()) {
        throw RtlException(Exception_FileException_Open, filename);
    }
    for (auto s: lines) {
        f << s.str() << "\n";   // Write line, and line-ending for each element in the array.
        if (f.fail()) {
            // If the write fails for any reason, consider that a FileException.Write exception.
            throw RtlException(Exception_FileException_Write, filename);
        }
    }
}

} // namespace file

} // namespace rtl
