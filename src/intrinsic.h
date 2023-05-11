#include "number.h"
#include "utf8string.h"

namespace rtl {

namespace ne_builtin {

utf8string string__concat(const utf8string &a, const utf8string &b);

} // namespace ne_builtin

namespace ne_global {

utf8string chr(Number x);
Number int_(Number a);
Number max(Number a, Number b);
Number min(Number a, Number b);
bool odd(Number x);
Number ord(const utf8string &s);
Number round(Number places, Number value);
utf8string str(Number x);

} // namespace ne_global

namespace ne_string {

utf8string quoted(const utf8string &s);

} // namespace ne_string

} // namespace rtl
