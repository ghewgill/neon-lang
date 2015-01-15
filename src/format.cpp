#include "format.h"

#include <utf8.h>

namespace format {

bool parse(const std::string &fmt, Spec &spec)
{
    spec.fill = ' ';
    spec.align = 0;
    spec.alternate = false;
    spec.sign = 0;
    spec.width = -1;
    spec.thousands = false;
    spec.precision = -1;
    spec.type = 0;
    if (fmt.empty()) {
        return true;
    }
    size_t i = 0;
    if (fmt[i] == '<' || fmt[i] == '>' || fmt[i] == '=' || fmt[i] == '^') {
        spec.align = fmt[i];
        ++i;
    }
    if (fmt[i] == '+' || fmt[i] == '-' || fmt[i] == ' ') {
        spec.sign = fmt[i];
        ++i;
    }
    if (fmt[i] == '0') {
        spec.fill = '0';
        if (spec.align == 0) {
            spec.align = '=';
        }
        ++i;
    }
    if (isdigit(fmt[i])) {
        size_t end = 0;
        spec.width = std::stoi(fmt.substr(i), &end, 10);
        i += end;
    }
    if (fmt[i] == ',') {
        spec.thousands = true;
        ++i;
    }
    if (fmt[i] == '.') {
        ++i;
        size_t end = 0;
        spec.precision = std::stoi(fmt.substr(i), &end, 10);
        i += end;
    }
    spec.type = fmt[i];
    switch (spec.type) {
        case 0:
        case 's':
            if (spec.sign || spec.alternate || spec.align == '=') {
                return false;
            }
            break;
        default:
            return false;
    }
    return true;
}

static std::string fillstr(const Spec &spec, int n)
{
    if (spec.fill < 128) {
        return std::string(n, static_cast<char>(spec.fill));
    }
    std::string r;
    while (n > 0) {
        utf8::append(spec.fill, std::back_inserter(r));
        n--;
    }
    return r;
}

std::string format(const std::string &str, const Spec &spec)
{
    std::string r = str;
    if (spec.precision >= 0 && static_cast<int>(r.length()) > spec.precision) {
        r = r.substr(0, spec.precision);
    }
    if (spec.width >= 0) {
        int len = static_cast<int>(r.length());
        if (len < spec.width) {
            int space = spec.width - len;
            if (spec.align == 0 || spec.align == '<') {
                r += fillstr(spec, space);
            } else if (spec.align == '>') {
                r = fillstr(spec, space) + r;
            } else if (spec.align == '^') {
                r = fillstr(spec, space/2) + r + fillstr(spec, space - space/2);
            }
        }
    }
    return r;
}

} // namespace format
