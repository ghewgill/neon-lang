#include "number.h"
#include "utf8string.h"

namespace rtl {

namespace global {

utf8string chr(Number x);
utf8string concat(const utf8string &a, const utf8string &b);
Number int_(Number a);
utf8string format(const utf8string &str, const utf8string &fmt);
Number max(Number a, Number b);
Number min(Number a, Number b);
Number num(const utf8string &s);
bool odd(Number x);
Number ord(const utf8string &s);
utf8string str(Number x);
utf8string strb(bool x);
utf8string substring(const utf8string &s, Number offset, Number length);

} // namespace global

} // namespace rtl
