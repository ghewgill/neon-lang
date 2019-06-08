#include <algorithm>
#include <iso646.h>
#include <string>
#include <vector>

#include "number.h"
#include "utf8string.h"

namespace rtl {

namespace ne_string {

Number find(const utf8string &s, const utf8string &t)
{
    std::string::size_type i = s.str().find(t.str());
    if (i == std::string::npos) {
        return number_from_sint64(-1);
    }
    return number_from_uint64(i);
}

utf8string lower(const utf8string &ss)
{
    const std::string &s = ss.str(); // TODO: utf8
    utf8string r;
    //std::transform(s.begin(), s.end(), std::back_inserter(r), ::tolower);
    for (auto c: s) {
        r.push_back(static_cast<char>(::tolower(c)));
    }
    return r;
}

std::vector<utf8string> split(const utf8string &ss, const utf8string &dd)
{
    const std::string &s = ss.str(); // TODO: utf8
    const std::string &d = dd.str(); // TODO: utf8
    std::vector<utf8string> r;
    std::string::size_type i = 0;
    while (i < s.length()) {
        std::string::size_type nd = s.find(d, i);
        if (nd == std::string::npos) {
            r.push_back(utf8string(s.substr(i)));
            break;
        } else if (nd > i) {
            r.push_back(utf8string(s.substr(i, nd-i)));
        }
        i = nd + d.length();
    }
    return r;
}

std::vector<utf8string> splitLines(const utf8string &ss)
{
    const std::string &s = ss.str(); // TODO: utf8
    std::vector<utf8string> r;
    std::string::size_type i = 0;
    while (i < s.length()) {
        std::string::size_type nl = s.find_first_of("\r\n", i);
        if (nl == std::string::npos) {
            r.push_back(utf8string(s.substr(i)));
            break;
        }
        r.push_back(utf8string(s.substr(i, nl-i)));
        if (s[nl] == '\r' && nl+1 < s.length() && s[nl+1] == '\n') {
            i = nl + 2;
        } else {
            i = nl + 1;
        }
    }
    return r;
}

utf8string trimCharacters(const utf8string &ss, const utf8string &trimLeadingChars, const utf8string &trimTrailingChars)
{
    const std::string &s = ss.str(); // TODO: utf8
    std::string::size_type first = s.find_first_not_of(trimLeadingChars.str());
    std::string::size_type last = s.find_last_not_of(trimTrailingChars.str());
    if (first == std::string::npos || last == std::string::npos) {
        return utf8string("");
    }
    return utf8string(s.substr(first, last-first+1));
}

utf8string upper(const utf8string &ss)
{
    const std::string &s = ss.str(); // TODO: utf8
    utf8string r;
    //std::transform(s.begin(), s.end(), std::back_inserter(r), ::toupper);
    for (auto c: s) {
        r.push_back(static_cast<char>(::toupper(c)));
    }
    return r;
}

} // namespace ne_string

} // namespace rtl
