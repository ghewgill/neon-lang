#include "debuginfo.h"

namespace {

std::vector<std::string> split(const std::string &s, char d)
{
    std::vector<std::string> r;
    std::string::size_type i = 0;
    auto start = i;
    while (i < s.length()) {
        if (s.at(i) == d) {
            r.push_back(s.substr(start, i-start));
            start = i + 1;
        }
        i++;
    }
    if (i > start) {
        r.push_back(s.substr(start, i-start));
    }
    return r;
}

} // namespace

DebugInfo::DebugInfo(const std::string &source)
  : source_lines(split(source, '\n')), line_numbers()
{
    source_lines.insert(source_lines.begin(), ""); // There is no line zero.
}
