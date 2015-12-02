#include "number.h"

namespace rtl {

std::string global$chr(Number x);
std::string global$concat(const std::string &a, const std::string &b);
Number global$int(Number a);
std::string global$format(const std::string &str, const std::string &fmt);
Number global$max(Number a, Number b);
Number global$min(Number a, Number b);
Number global$num(const std::string &s);
Number global$ord(const std::string &s);
std::string global$str(Number x);
std::string global$strb(bool x);
std::string global$substring(const std::string &s, Number offset, Number length);

} // namespace rtl
