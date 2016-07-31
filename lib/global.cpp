#include <assert.h>
#include <iso646.h>
#include <iostream>
#include <sstream>

#include <utf8.h>

#include "cell.h"
#include "format.h"
#include "number.h"
#include "rtl_exec.h"

namespace rtl {

namespace global {

void array__append(Cell *self, Cell &element)
{
    self->array_for_write().push_back(element);
}

Cell array__concat(Cell &left, Cell &right)
{
    std::vector<Cell> a = left.array();
    const std::vector<Cell> &b = right.array();
    std::copy(b.begin(), b.end(), std::back_inserter(a));
    return Cell(a);
}

void array__extend(Cell *self, Cell &elements)
{
    std::copy(elements.array().begin(), elements.array().end(), std::back_inserter(self->array_for_write()));
}

std::vector<Number> array__range(Number first, Number last, Number step)
{
    std::vector<Number> r;
    if (number_is_zero(step)) {
        throw RtlException(Exception_ValueRangeException, number_to_string(step));
    }
    if (number_is_negative(step)) {
        for (Number i = first; number_is_greater_equal(i, last); i = number_add(i, step)) {
            r.push_back(i);
        }
    } else {
        for (Number i = first; number_is_less_equal(i, last); i = number_add(i, step)) {
            r.push_back(i);
        }
    }
    return r;
}

void array__resize(Cell *self, Number new_size)
{
    if (not number_is_integer(new_size)) {
        throw RtlException(Exception_ArrayIndexException, number_to_string(new_size));
    }
    self->array_for_write().resize(number_to_sint64(new_size));
}

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
    if (lst < -1) lst = -1;
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
    if (lst < -1) lst = -1;
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

std::string array__toBytes__number(const std::vector<Number> &a)
{
    std::string r;
    r.reserve(a.size());
    for (auto x: a) {
        uint64_t b = number_to_uint64(x);
        if (b >= 256) {
            throw RtlException(Exception_ByteOutOfRangeException, std::to_string(b));
        }
        r.push_back(static_cast<unsigned char>(b));
    }
    return r;
}

std::string array__toString__number(const std::vector<Number> &a)
{
    std::string r = "[";
    for (Number x: a) {
        if (r.length() > 1) {
            r.append(", ");
        }
        r.append(number_to_string(x));
    }
    r.append("]");
    return r;
}

std::string array__toString__string(const std::vector<utf8string> &a)
{
    std::string r = "[";
    for (utf8string x: a) {
        if (r.length() > 1) {
            r.append(", ");
        }
        // TODO: Escape embedded quotes as necessary.
        r.append("\"" + x.str() + "\"");
    }
    r.append("]");
    return r;
}

std::string boolean__toString(bool self)
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

std::string number__toString(Number self)
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

std::string string__toBytes(const std::string &self)
{
    return self;
}

std::string bytes__range(const std::string &t, Number first, bool first_from_end, Number last, bool last_from_end)
{
    assert(number_is_integer(first));
    assert(number_is_integer(last));
    int64_t f = number_to_sint64(first);
    int64_t l = number_to_sint64(last);
    if (first_from_end) {
        f += t.size() - 1;
    }
    if (last_from_end) {
        l += t.size() - 1;
    }
    return t.substr(f, l + 1 - f);
}

Number bytes__size(const std::string &self)
{
    return number_from_uint64(self.length());
}

std::string bytes__splice(const std::string &t, const std::string &s, Number first, bool first_from_end, Number last, bool last_from_end)
{
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

std::string bytes__decodeToString(const std::string &self)
{
    auto inv = utf8::find_invalid(self.begin(), self.end());
    if (inv != self.end()) {
        throw RtlException(Exception_Utf8DecodingException, std::to_string(std::distance(self.begin(), inv)));
    }
    return self;
}

std::vector<Number> bytes__toArray(const std::string &self)
{
    std::vector<Number> r;
    for (auto x: self) {
        r.push_back(number_from_uint8(x));
    }
    return r;
}

std::string bytes__toString(const std::string &self)
{
    std::stringstream r;
    r << "HEXBYTES \"";
    bool first = true;
    for (char c: self) {
        if (first) {
            first = false;
        } else {
            r << ' ';
        }
        static const char hex[] = "0123456789abcdef";
        r << hex[(c >> 4) & 0xf];
        r << hex[c & 0xf];
    }
    r << "\"";
    return r.str();
}

std::string pointer__toString(void *p)
{
    return "<p:" + std::to_string(reinterpret_cast<intptr_t>(p)) + ">";
}

std::string functionpointer__toString(Cell &p)
{
    return "<fp:ip=" + number_to_string(p.number()) + ">";
}

std::string concatBytes(const std::string &a, const std::string &b)
{
    return a + b;
}

std::string input(const std::string &prompt)
{
    std::cout << prompt;
    std::string r;
    if (not std::getline(std::cin, r)) {
        throw RtlException(Exception_EndOfFileException, "");
    }
    return r;
}

void print(const std::string &s)
{
    std::cout << s << "\n";
}

} // namespace global

} // namespace rtl
