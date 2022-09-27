#include "object.h"

#include <iso646.h>

#include "rtl_exec.h"
#include "intrinsic.h"

bool ObjectString::invokeMethod(const utf8string &name, const std::vector<std::shared_ptr<Object>> &args, std::shared_ptr<Object> &result) const
{
    std::string method = name.str();
    if (method == "length") {
        if (args.size() != 0) {
            throw RtlException(rtl::ne_global::Exception_DynamicConversionException, utf8string("invalid number of arguments to length() (expected 0)"));
        }
        result = std::shared_ptr<Object> { new ObjectNumber(number_from_uint64(s.length())) };
        return true;
    }
    throw RtlException(rtl::ne_global::Exception_DynamicConversionException, utf8string("string object does not support this method"));
}

utf8string ObjectString::toLiteralString() const
{
    return rtl::ne_string::quoted(s);
}

bool ObjectArray::invokeMethod(const utf8string &name, const std::vector<std::shared_ptr<Object>> &args, std::shared_ptr<Object> &result) const
{
    std::string method = name.str();
    if (method == "size") {
        if (args.size() != 0) {
            throw RtlException(rtl::ne_global::Exception_DynamicConversionException, utf8string("invalid number of arguments to size() (expected 0)"));
        }
        result = std::shared_ptr<Object> { new ObjectNumber(number_from_uint64(a.size())) };
        return true;
    }
    throw RtlException(rtl::ne_global::Exception_DynamicConversionException, utf8string("array object does not support this method"));
}

bool ObjectArray::subscript(std::shared_ptr<Object> index, std::shared_ptr<Object> &r) const
{
    Number i;
    if (not index->getNumber(i)) {
        throw RtlException(rtl::ne_global::Exception_DynamicConversionException, utf8string("to Number"));
    }
    uint64_t ii = number_to_uint64(i);
    if (ii >= a.size()) {
        throw PanicException(utf8string("Array index exceeds size " + std::to_string(a.size()) + ": " + number_to_string(i)));
    }
    r = a.at(ii);
    return true;
}

utf8string ObjectArray::toString() const
{
    UTF8StringBuilder r {"["};
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
    return utf8string(r);
}

bool ObjectDictionary::invokeMethod(const utf8string &name, const std::vector<std::shared_ptr<Object>> &args, std::shared_ptr<Object> &result) const
{
    std::string method = name.str();
    if (method == "keys") {
        if (args.size() != 0) {
            throw RtlException(rtl::ne_global::Exception_DynamicConversionException, utf8string("invalid number of arguments to keys() (expected 0)"));
        }
        std::vector<std::shared_ptr<Object>> keys;
        for (auto &x: d) {
            keys.push_back(std::shared_ptr<Object> { new ObjectString(x.first) });
        }
        result = std::shared_ptr<Object> { new ObjectArray(keys) };
        return true;
    }
    if (method == "size") {
        if (args.size() != 0) {
            throw RtlException(rtl::ne_global::Exception_DynamicConversionException, utf8string("invalid number of arguments to size() (expected 0)"));
        }
        result = std::shared_ptr<Object> { new ObjectNumber(number_from_uint64(d.size())) };
        return true;
    }
    throw RtlException(rtl::ne_global::Exception_DynamicConversionException, utf8string("dictionary object does not support this method"));
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
    UTF8StringBuilder r {"{"};
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
    return utf8string(r);
}
