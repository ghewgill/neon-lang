#ifndef FORMAT_H
#define FORMAT_H

#include <stdint.h>
#include <string>

namespace format {

struct Spec {
    uint32_t fill;
    char align;
    bool alternate;
    char sign;
    int width;
    bool thousands;
    int precision;
    char type;
};

bool parse(const std::string &fmt, Spec &spec);
std::string format(const std::string &str, const Spec &spec);

} // namespace format

#endif
