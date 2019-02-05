#include "number.h"
#include "utf8string.h"

namespace rtl {

namespace global {

utf8string chr(Number x);
utf8string concat(const utf8string &a, const utf8string &b);
Number int_(Number a);
Number max(Number a, Number b);
Number min(Number a, Number b);
Number num(const utf8string &s);
bool odd(Number x);
Number ord(const utf8string &s);
Number round(Number places, Number value);
utf8string str(Number x);

} // namespace global

} // namespace rtl
