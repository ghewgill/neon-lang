#include <algorithm>
#include <iso646.h>
#include <string>
#include <vector>

#include "number.h"
#include "utf8string.h"

namespace rtl {

Number string$find(const std::string &s, const std::string &t)
{
    std::string::size_type i = s.find(t);
    if (i == std::string::npos) {
        return number_from_sint64(-1);
    }
    return number_from_uint64(i);
}

bool string$hasPrefix(const std::string &s, const std::string &prefix)
{
    return s.substr(0, prefix.length()) == prefix;
}

bool string$hasSuffix(const std::string &s, const std::string &suffix)
{
    return s.length() >= suffix.length() && s.substr(s.length() - suffix.length()) == suffix;
}

std::string string$join(const std::vector<utf8string> &a, const std::string &d)
{
    std::string r;
    for (auto s: a) {
        if (not r.empty()) {
            r.append(d);
        }
        r.append(s.str());
    }
    return r;
}

std::string string$lower(const std::string &s)
{
    std::string r;
    std::transform(s.begin(), s.end(), std::back_inserter(r), ::tolower);
    return r;
}

std::vector<utf8string> string$split(const std::string &s, const std::string &d)
{
    std::vector<utf8string> r;
    std::string::size_type i = 0;
    while (i < s.length()) {
        std::string::size_type nd = s.find(d, i);
        if (nd == std::string::npos) {
            r.push_back(s.substr(i));
            break;
        } else if (nd > i) {
            r.push_back(s.substr(i, nd-i));
        }
        i = nd + d.length();
    }
    return r;
}

std::string string$trim(const std::string &s)
{
    std::string::size_type first = s.find_first_not_of(' ');
    std::string::size_type last = s.find_last_not_of(' ');
    if (first == std::string::npos || last == std::string::npos) {
        return "";
    }
    return s.substr(first, last-first+1);
}

std::string string$upper(const std::string &s)
{
    std::string r;
    std::transform(s.begin(), s.end(), std::back_inserter(r), ::toupper);
    return r;
}

}
