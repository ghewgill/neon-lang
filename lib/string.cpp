#include <algorithm>
#include <iso646.h>
#include <string>
#include <vector>

#include "cell.h"
#include "intrinsic.h"
#include "number.h"
#include "utf8string.h"

#include "choices.inc"

namespace rtl {

namespace ne_string {

Cell find(const utf8string &s, const utf8string &t)
{
    std::string::size_type i = s.str().find(t.str());
    if (i == std::string::npos) {
        return Cell(std::vector<Cell> {
            Cell(number_from_uint32(CHOICE_FindResult_notfound))
        });
    }
    return Cell(std::vector<Cell> {
        Cell(number_from_uint32(CHOICE_FindResult_index)),
        Cell(number_from_uint32(i))
    });
}

utf8string fromCodePoint(Number code)
{
    return ne_global::chr(code);
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

utf8string quoted(const utf8string &s)
{
    utf8string r;
    r.push_back('"');
    for (auto i = s.str().begin(); i != s.str().end(); utf8::advance(i, 1, s.str().end())) {
        auto c = utf8::peek_next(i, s.str().end());
        switch (c) {
            case '\b': r.append("\\b"); break;
            case '\f': r.append("\\f"); break;
            case '\n': r.append("\\n"); break;
            case '\r': r.append("\\r"); break;
            case '\t': r.append("\\t"); break;
            case '"':
            case '\\':
                r.push_back('\\');
                r.push_back(static_cast<char>(c));
                break;
            default:
                if (c >= ' ' && c < 0x7f) {
                    r.push_back(static_cast<char>(c));
                } else if (c < 0x10000) {
                    char buf[7];
                    snprintf(buf, sizeof(buf), "\\u%04x", c);
                    r.append(buf);
                } else {
                    char buf[11];
                    snprintf(buf, sizeof(buf), "\\U%08x", c);
                    r.append(buf);
                }
                break;
        }
    }
    r.push_back('"');
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

Number toCodePoint(const utf8string &s)
{
    return ne_global::ord(s);
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
