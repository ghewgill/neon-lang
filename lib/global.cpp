#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#ifndef snprintf
#define snprintf _snprintf
#endif
#endif
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

std::vector<unsigned char> array__toBytes__number(const std::vector<Number> &a)
{
    std::vector<unsigned char> r;
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

std::string array__toString__object(std::vector<std::shared_ptr<Object>> a)
{
    std::string r = "[";
    for (auto &x: a) {
        if (r.length() > 1) {
            r.append(", ");
        }
        r.append(x->toString());
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

std::vector<unsigned char> string__toBytes(const std::string &self)
{
    return std::vector<unsigned char>(self.data(), self.data()+self.size());
}

std::string string__toString(const std::string &self)
{
    // TODO: Escape things that need escaping.
    return "\"" + self + "\"";
}

std::vector<unsigned char> bytes__range(const std::vector<unsigned char> &t, Number first, bool first_from_end, Number last, bool last_from_end)
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
    std::vector<unsigned char> r;
    std::copy(t.begin()+f, t.begin()+l+1, std::back_inserter(r));
    return r;
}

Number bytes__size(const std::vector<unsigned char> &self)
{
    return number_from_uint64(self.size());
}

std::vector<unsigned char> bytes__splice(const std::vector<unsigned char> &t, const std::vector<unsigned char> &s, Number first, bool first_from_end, Number last, bool last_from_end)
{
    int64_t f = number_to_sint64(first);
    int64_t l = number_to_sint64(last);
    if (first_from_end) {
        f += s.size() - 1;
    }
    if (last_from_end) {
        l += s.size() - 1;
    }
    std::vector<unsigned char> r;
    std::copy(s.begin(), s.begin()+f, std::back_inserter(r));
    std::copy(t.begin(), t.end(), std::back_inserter(r));
    std::copy(s.begin()+l+1, s.end(), std::back_inserter(r));
    return r;
}

std::string bytes__decodeToString(const std::vector<unsigned char> &self)
{
    auto inv = utf8::find_invalid(self.begin(), self.end());
    if (inv != self.end()) {
        throw RtlException(Exception_Utf8DecodingException, std::to_string(std::distance(self.begin(), inv)));
    }
    return std::string(self.begin(), self.end());
}

std::vector<Number> bytes__toArray(const std::vector<unsigned char> &self)
{
    std::vector<Number> r;
    for (auto x: self) {
        r.push_back(number_from_uint8(x));
    }
    return r;
}

std::string bytes__toString(const std::vector<unsigned char> &self)
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

std::string interfacepointer__toString(Cell &p)
{
    return "<ip:" + std::to_string(reinterpret_cast<intptr_t>(p.array_for_write()[0].address())) + "," + number_to_string(p.array_for_write()[1].number()) + ">";
}

std::vector<unsigned char> concatBytes(const std::vector<unsigned char> &a, const std::vector<unsigned char> &b)
{
    std::vector<unsigned char> r = a;
    std::copy(b.begin(), b.end(), std::back_inserter(r));
    return r;
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

std::shared_ptr<Object> object__makeNull()
{
    return std::shared_ptr<Object>(nullptr);
}

class ObjectBoolean: public Object {
public:
    ObjectBoolean(bool b): b(b) {}
    virtual bool getBoolean(bool &r) const override { r = b; return true; }
    virtual std::string toString() const override { return b ? "TRUE" : "FALSE"; }
private:
    const bool b;
private:
    ObjectBoolean(const ObjectBoolean &);
    ObjectBoolean &operator=(const ObjectBoolean &);
};

std::shared_ptr<Object> object__makeBoolean(bool b)
{
    return std::shared_ptr<Object>(new ObjectBoolean(b));
}

class ObjectNumber: public Object {
public:
    ObjectNumber(Number n): n(n) {}
    virtual bool getNumber(Number &r) const override { r = n; return true; }
    virtual std::string toString() const override { return number_to_string(n); }
private:
    const Number n;
private:
    ObjectNumber(const ObjectNumber &);
    ObjectNumber &operator=(const ObjectNumber &);
};

std::shared_ptr<Object> object__makeNumber(Number n)
{
    return std::shared_ptr<Object>(new ObjectNumber(n));
}

class ObjectString: public Object {
public:
    ObjectString(const std::string &s): s(s) {}
    virtual bool getString(std::string &r) const override { r = s; return true; }
    virtual std::string toString() const override { return "\"" + s + "\""; }
private:
    const std::string s;
private:
    ObjectString(const ObjectString &);
    ObjectString &operator=(const ObjectString &);
};

std::shared_ptr<Object> object__makeString(const std::string &s)
{
    return std::shared_ptr<Object>(new ObjectString(s));
}

class ObjectBytes: public Object {
public:
    ObjectBytes(const std::vector<unsigned char> &b): b(b) {}
    virtual bool getBytes(std::vector<unsigned char> &r) const override { r = b; return true; }
    virtual std::string toString() const override {
        std::string r = "HEXBYTES \"";
        bool first = true;
        for (auto x: b) {
            if (first) {
                first = false;
            } else {
                r += ' ';
            }
            char buf[3];
            snprintf(buf, sizeof(buf), "%02x", x);
        }
        r += "\"";
        return r;
    }
private:
    const std::vector<unsigned char> b;
private:
    ObjectBytes(const ObjectBytes &);
    ObjectBytes &operator=(const ObjectBytes &);
};

std::shared_ptr<Object> object__makeBytes(const std::vector<unsigned char> &b)
{
    return std::shared_ptr<Object>(new ObjectBytes(b));
}

class ObjectArray: public Object {
public:
    ObjectArray(std::vector<std::shared_ptr<Object>> a): a(a) {}
    virtual bool getArray(std::vector<std::shared_ptr<Object>> &r) const override { r = a; return true; }
    virtual bool subscript(std::shared_ptr<Object> index, std::shared_ptr<Object> &r) const override {
        Number i;
        if (not index->getNumber(i)) {
            throw RtlException(Exception_DynamicConversionException, "to Number");
        }
        uint64_t ii = number_to_uint64(i);
        if (ii >= a.size()) {
            throw RtlException(Exception_ArrayIndexException, number_to_string(i));
        }
        r = a.at(ii);
        return true;
    }
    virtual std::string toString() const override {
        std::string r = "[";
        bool first = true;
        for (auto x: a) {
            if (not first) {
                r.append(", ");
            } else {
                first = false;
            }
            r.append(x->toString());
        }
        r.append("]");
        return r;
    }
private:
    std::vector<std::shared_ptr<Object>> a;
private:
    ObjectArray(const ObjectArray &);
    ObjectArray &operator=(const ObjectArray &);
};

std::shared_ptr<Object> object__makeArray(std::vector<std::shared_ptr<Object>> a)
{
    return std::shared_ptr<Object>(new ObjectArray(a));
}

class ObjectDictionary: public Object {
public:
    ObjectDictionary(std::map<utf8string, std::shared_ptr<Object>> d): d(d) {}
    virtual bool getDictionary(std::map<utf8string, std::shared_ptr<Object>> &r) const override { r = d; return true; }
    virtual bool subscript(std::shared_ptr<Object> index, std::shared_ptr<Object> &r) const override {
        std::string i;
        if (not index->getString(i)) {
            throw RtlException(Exception_DynamicConversionException, "to String");
        }
        auto e = d.find(i);
        if (e == d.end()) {
            return false;
        }
        r = e->second;
        return true;
    }
    virtual std::string toString() const override {
        std::string r = "{";
        bool first = true;
        for (auto x: d) {
            if (not first) {
                r.append(", ");
            } else {
                first = false;
            }
            r.append("\"");
            r.append(x.first.c_str());
            r.append("\": ");
            r.append(x.second->toString());
        }
        r.append("}");
        return r;
    }
private:
    std::map<utf8string, std::shared_ptr<Object>> d;
private:
    ObjectDictionary(const ObjectDictionary &);
    ObjectDictionary &operator=(const ObjectDictionary &);
};

std::shared_ptr<Object> object__makeDictionary(std::map<utf8string, std::shared_ptr<Object>> d)
{
    return std::shared_ptr<Object>(new ObjectDictionary(d));
}

bool object__getBoolean(std::shared_ptr<Object> obj)
{
    bool r;
    if (obj == nullptr || not obj->getBoolean(r)) {
        throw RtlException(Exception_DynamicConversionException, "to Boolean");
    }
    return r;
}

Number object__getNumber(std::shared_ptr<Object> obj)
{
    Number r;
    if (obj == nullptr || not obj->getNumber(r)) {
        throw RtlException(Exception_DynamicConversionException, "to Number");
    }
    return r;
}

std::string object__getString(std::shared_ptr<Object> obj)
{
    std::string r;
    if (obj == nullptr || not obj->getString(r)) {
        throw RtlException(Exception_DynamicConversionException, "to String");
    }
    return r;
}

std::vector<unsigned char> object__getBytes(std::shared_ptr<Object> obj)
{
    std::vector<unsigned char> r;
    if (obj == nullptr || not obj->getBytes(r)) {
        throw RtlException(Exception_DynamicConversionException, "to Bytes");
    }
    return r;
}

std::vector<std::shared_ptr<Object>> object__getArray(std::shared_ptr<Object> obj)
{
    std::vector<std::shared_ptr<Object>> r;
    if (obj == nullptr || not obj->getArray(r)) {
        throw RtlException(Exception_DynamicConversionException, "to Array");
    }
    return r;
}

std::map<utf8string, std::shared_ptr<Object>> object__getDictionary(std::shared_ptr<Object> obj)
{
    std::map<utf8string, std::shared_ptr<Object>> r;
    if (obj == nullptr || not obj->getDictionary(r)) {
        throw RtlException(Exception_DynamicConversionException, "to Dictionary");
    }
    return r;
}

bool object__isNull(std::shared_ptr<Object> obj)
{
    return obj == nullptr;
}

std::string object__toString(std::shared_ptr<Object> obj)
{
    if (obj == nullptr) {
        return "null";
    }
    return obj->toString();
}

std::shared_ptr<Object> object__subscript(std::shared_ptr<Object> obj, std::shared_ptr<Object> index)
{
    if (obj == nullptr) {
        throw RtlException(Exception_DynamicConversionException, "object is null");
    }
    std::shared_ptr<Object> r;
    if (obj == nullptr || not obj->subscript(index, r)) {
        throw RtlException(Exception_ObjectSubscriptException, index->toString());
    }
    return r;
}

} // namespace global

} // namespace rtl
