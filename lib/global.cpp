#include <assert.h>
#include <iso646.h>
#include <iostream>

#include <utf8.h>

#include "cell.h"
#include "format.h"
#include "number.h"
#include "rtl_exec.h"

namespace rtl {

void global$array__append(Cell *self, Cell &element)
{
    self->array_for_write().push_back(element);
}

Cell global$array__concat(Cell &left, Cell &right)
{
    std::vector<Cell> a = left.array();
    const std::vector<Cell> &b = right.array();
    std::copy(b.begin(), b.end(), std::back_inserter(a));
    return Cell(a);
}

void global$array__extend(Cell *self, Cell &elements)
{
    std::copy(elements.array().begin(), elements.array().end(), std::back_inserter(self->array_for_write()));
}

void global$array__resize(Cell *self, Number new_size)
{
    if (not number_is_integer(new_size)) {
        throw RtlException(Exception_global$ArrayIndex, number_to_string(new_size));
    }
    self->array_for_write().resize(number_to_sint64(new_size));
}

Number global$array__size(Cell &self)
{
    return number_from_uint64(self.array().size());
}

Cell global$array__slice(Cell &a, Number first, bool first_from_end, Number last, bool last_from_end)
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

Cell global$array__splice(Cell &b, Cell &a, Number first, bool first_from_end, Number last, bool last_from_end)
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

std::string global$array__toBytes__number(const std::vector<Number> &a)
{
    std::string r;
    r.reserve(a.size());
    for (auto x: a) {
        uint64_t b = number_to_uint64(x);
        if (b >= 256) {
            throw RtlException(Exception_global$ByteOutOfRange, std::to_string(b));
        }
        r.push_back(static_cast<unsigned char>(b));
    }
    return r;
}

std::string global$array__toString__number(const std::vector<Number> &a)
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

std::string global$array__toString__string(const std::vector<utf8string> &a)
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

std::string global$boolean__toString(bool self)
{
    return self ? "TRUE" : "FALSE";
}

Number global$dictionary__size(Cell &self)
{
    return number_from_uint64(self.dictionary().size());
}

std::vector<utf8string> global$dictionary__keys(Cell &self)
{
    std::vector<utf8string> r;
    for (auto d: self.dictionary()) {
        r.push_back(d.first);
    }
    return r;
}

std::string global$number__toString(Number self)
{
    return number_to_string(self);
}

void global$string__append(utf8string *self, const std::string &t)
{
    self->append(t);
}

Number global$string__length(const std::string &self)
{
    return number_from_uint64(self.length());
}

std::string global$string__splice(const std::string &t, const std::string &s, Number first, bool first_from_end, Number last, bool last_from_end)
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

std::string global$string__substring(const std::string &t, Number first, bool first_from_end, Number last, bool last_from_end)
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

std::string global$string__toBytes(const std::string &self)
{
    return self;
}

std::string global$bytes__range(const std::string &t, Number first, bool first_from_end, Number last, bool last_from_end)
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

Number global$bytes__size(const std::string &self)
{
    return number_from_uint64(self.length());
}

std::string global$bytes__splice(const std::string &t, const std::string &s, Number first, bool first_from_end, Number last, bool last_from_end)
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

std::vector<Number> global$bytes__toArray(const std::string &self)
{
    std::vector<Number> r;
    for (auto x: self) {
        r.push_back(number_from_uint8(x));
    }
    return r;
}

std::string global$bytes__toString(const std::string &self)
{
    auto inv = utf8::find_invalid(self.begin(), self.end());
    if (inv != self.end()) {
        throw RtlException(Exception_global$Utf8Encoding, "");
    }
    return self;
}

std::string global$pointer__toString(void *p)
{
    return "<p:" + std::to_string(reinterpret_cast<intptr_t>(p)) + ">";
}

std::string global$functionpointer__toString(Cell &p)
{
    return "<fp:ip=" + number_to_string(p.number()) + ">";
}

std::string global$concatBytes(const std::string &a, const std::string &b)
{
    return a + b;
}

std::string global$input(const std::string &prompt)
{
    std::cout << prompt;
    std::string r;
    if (not std::getline(std::cin, r)) {
        throw RtlException(Exception_global$EndOfFile, "");
    }
    return r;
}

void global$print(const std::string &s)
{
    std::cout << s << "\n";
}

} // namespace rtl
