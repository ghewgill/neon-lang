#include <assert.h>
#include <iso646.h>
#include <iostream>

#include <utf8.h>

#include "cell.h"
#include "format.h"
#include "number.h"
#include "rtl_exec.h"

namespace rtl {

Number array__size(Cell &self)
{
    return number_from_uint64(self.array().size());
}

Cell array__slice(Cell &a, Number first, bool first_from_end, Number last, bool last_from_end)
{
    const std::vector<Cell> &array = a.array();
    int64_t fst = number_to_sint64(first);
    int64_t lst = number_to_sint64(last);
    if (first_from_end) {
        fst += array.size() - 1;
    }
    if (fst < 0) fst = 0;
    if (fst > static_cast<int64_t>(array.size())) fst = array.size();
    if (last_from_end) {
        lst += array.size() - 1;
    }
    if (lst < 0) lst = 0;
    if (lst >= static_cast<int64_t>(array.size())) lst = array.size() - 1;
    std::vector<Cell> r;
    for (auto i = fst; i <= lst; i++) {
        r.push_back(array[i]);
    }
    return Cell(r);
}

Cell array__splice(Cell &b, Cell &a, Number first, bool first_from_end, Number last, bool last_from_end)
{
    const std::vector<Cell> &array = a.array();
    int64_t fst = number_to_sint64(first);
    int64_t lst = number_to_sint64(last);
    if (first_from_end) {
        fst += array.size() - 1;
    }
    if (fst < 0) fst = 0;
    if (fst > static_cast<int64_t>(array.size())) fst = array.size();
    if (last_from_end) {
        lst += array.size() - 1;
    }
    if (lst < 0) lst = 0;
    if (lst >= static_cast<int64_t>(array.size())) lst = array.size() - 1;
    std::vector<Cell> r;
    for (auto i = 0; i < fst; i++) {
        r.push_back(array[i]);
    }
    std::copy(b.array().begin(), b.array().end(), std::back_inserter(r));
    for (auto i = lst + 1; i < static_cast<int64_t>(array.size()); i++) {
        r.push_back(array[i]);
    }
    return Cell(r);
}

std::string boolean__to_string(bool self)
{
    return self ? "TRUE" : "FALSE";
}

Number dictionary__size(Cell &self)
{
    return number_from_uint64(self.dictionary().size());
}

std::vector<utf8string> dictionary__keys(Cell &self)
{
    std::vector<utf8string> r;
    for (auto d: self.dictionary()) {
        r.push_back(d.first);
    }
    return r;
}

std::string number__to_string(Number self)
{
    return number_to_string(self);
}

void string__append(utf8string *self, const std::string &t)
{
    self->append(t);
}

Number string__length(const std::string &self)
{
    return number_from_uint64(self.length());
}

std::string string__splice(const std::string &t, const std::string &s, Number first, bool first_from_end, Number last, bool last_from_end)
{
    // TODO: utf8
    int64_t f = number_to_sint64(first);
    int64_t l = number_to_sint64(last);
    if (first_from_end) {
        f += s.size() - 1;
    }
    if (last_from_end) {
        l += s.size() - 1;
    }
    return s.substr(0, f) + t + s.substr(l + 1);
}

std::string string__substring(const std::string &t, Number first, bool first_from_end, Number last, bool last_from_end)
{
    const utf8string &s = reinterpret_cast<const utf8string &>(t);
    assert(number_is_integer(first));
    assert(number_is_integer(last));
    int64_t f = number_to_sint64(first);
    int64_t l = number_to_sint64(last);
    if (first_from_end) {
        f += s.size() - 1;
    }
    if (last_from_end) {
        l += s.size() - 1;
    }
    size_t start = s.index(f);
    size_t end = s.index(l + 1);
    return s.substr(start, end-start);
}

void bytes__from_array(utf8string *self, const std::vector<Number> &a)
{
    self->clear();
    self->reserve(a.size());
    for (auto x: a) {
        uint64_t b = number_to_uint64(x);
        if (b >= 256) {
            throw RtlException("ByteOutOfRange", std::to_string(b));
        }
        self->push_back(static_cast<unsigned char>(b));
    }
}

Number bytes__size(const std::string &self)
{
    return number_from_uint64(self.length());
}

std::vector<Number> bytes__to_array(const std::string &self)
{
    std::vector<Number> r;
    for (auto x: self) {
        r.push_back(number_from_uint8(x));
    }
    return r;
}

std::string chr(Number x)
{
    assert(number_is_integer(x));
    std::string r;
    utf8::append(number_to_uint32(x), std::back_inserter(r));
    return r;
}

std::string concat(const std::string &a, const std::string &b)
{
    return a + b;
}

void dec(Number *a)
{
    *a = number_subtract(*a, number_from_uint32(1));
}

void inc(Number *a)
{
    *a = number_add(*a, number_from_uint32(1));
}

std::string format(const std::string &str, const std::string &fmt)
{
    format::Spec spec;
    if (not format::parse(fmt, spec)) {
        throw RtlException("FormatException", fmt);
    }
    return format::format(str, spec);
}

std::string input(const std::string &prompt)
{
    std::cout << prompt;
    std::string r;
    std::getline(std::cin, r);
    return r;
}

Number max(Number a, Number b)
{
    if (number_is_greater(a, b)) {
        return a;
    } else {
        return b;
    }
}

Number min(Number a, Number b)
{
    if (number_is_less(a, b)) {
        return a;
    } else {
        return b;
    }
}

Number num(const std::string &s)
{
    return number_from_string(s);
}

Number ord(const std::string &s)
{
    assert(utf8::distance(s.begin(), s.end()) == 1);
    auto it = s.begin();
    return number_from_uint32(utf8::next(it, s.end()));
}

void print(const std::string &s)
{
    std::cout << s << "\n";
}

std::string str(Number x)
{
    return number_to_string(x);
}

std::string strb(bool x)
{
    return x ? "TRUE" : "FALSE";
}

std::string substring(const std::string &s, Number offset, Number length)
{
    assert(number_is_integer(offset));
    assert(number_is_integer(length));
    auto start = s.begin();
    utf8::advance(start, number_to_uint32(offset), s.end());
    auto end = start;
    utf8::advance(end, number_to_uint32(length), s.end());
    return std::string(start, end);
}

} // namespace rtl
