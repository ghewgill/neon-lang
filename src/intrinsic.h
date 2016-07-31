#include "number.h"

namespace rtl {

namespace global {

std::string chr(Number x);
std::string concat(const std::string &a, const std::string &b);
Number int_(Number a);
std::string format(const std::string &str, const std::string &fmt);
Number max(Number a, Number b);
Number min(Number a, Number b);
Number num(const std::string &s);
Number ord(const std::string &s);
std::string str(Number x);
std::string strb(bool x);
std::string substring(const std::string &s, Number offset, Number length);

} // namespace global

} // namespace rtl
