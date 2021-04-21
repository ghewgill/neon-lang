#include "object.h"

#include <iso646.h>

#include "rtl_exec.h"
#include "intrinsic.h"

utf8string ObjectString::toLiteralString() const
{
    return rtl::ne_string::quoted(s);
}

bool ObjectArray::subscript(std::shared_ptr<Object> index, std::shared_ptr<Object> &r) const
{
    Number i;
    if (not index->getNumber(i)) {
        throw RtlException(rtl::ne_global::Exception_DynamicConversionException, utf8string("to Number"));
    }
    uint64_t ii = number_to_uint64(i);
    if (ii >= a.size()) {
        throw RtlException(rtl::ne_global::Exception_ArrayIndexException, utf8string(number_to_string(i)));
    }
    r = a.at(ii);
    return true;
}

utf8string ObjectArray::toString() const
{
    utf8string r {"["};
    bool first = true;
    for (auto x: a) {
        if (not first) {
            r.append(", ");
        } else {
            first = false;
        }
        r.append(x != nullptr ? x->toLiteralString() : utf8string("null"));
    }
    r.append("]");
    return r;
}

bool ObjectDictionary::subscript(std::shared_ptr<Object> index, std::shared_ptr<Object> &r) const
{
    utf8string i;
    if (not index->getString(i)) {
        throw RtlException(rtl::ne_global::Exception_DynamicConversionException, utf8string("to String"));
    }
    auto e = d.find(i);
    if (e == d.end()) {
        return false;
    }
    r = e->second;
    return true;
}

utf8string ObjectDictionary::toString() const
{
    utf8string r {"{"};
    bool first = true;
    for (auto x: d) {
        if (not first) {
            r.append(", ");
        } else {
            first = false;
        }
        r.append(rtl::ne_string::quoted(x.first));
        r.append(": ");
        r.append(x.second != nullptr ? x.second->toLiteralString() : utf8string("null"));
    }
    r.append("}");
    return r;
}
