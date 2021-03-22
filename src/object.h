#ifndef OBJECT_H
#define OBJECT_H

#include <map>
#include <memory>

#include "number.h"
#include "utf8string.h"

class Object {
public:
    virtual ~Object() {}
    virtual bool getBoolean(bool &) const { return false; }
    virtual bool getNumber(Number &) const { return false; }
    virtual bool getString(utf8string &) const { return false; }
    virtual bool getBytes(std::vector<unsigned char> &) const { return false; }
    virtual bool getArray(std::vector<std::shared_ptr<Object>> &) const { return false; }
    virtual bool getDictionary(std::map<utf8string, std::shared_ptr<Object>> &) const { return false; }
    virtual bool invokeMethod(const utf8string &/*name*/, const std::vector<std::shared_ptr<Object>> &/*args*/, std::shared_ptr<Object> &/*result*/) const { return false; }
    virtual bool setProperty(std::shared_ptr<Object>, std::shared_ptr<Object>) const { return false; }
    virtual bool subscript(std::shared_ptr<Object>, std::shared_ptr<Object> &) const { return false; }
    virtual utf8string toLiteralString() const { return toString(); }
    virtual utf8string toString() const = 0;
};

class ObjectBoolean: public Object {
public:
    explicit ObjectBoolean(bool b): b(b) {}
    virtual bool getBoolean(bool &r) const override { r = b; return true; }
    virtual utf8string toString() const override { return utf8string(b ? "TRUE" : "FALSE"); }
private:
    const bool b;
private:
    ObjectBoolean(const ObjectBoolean &);
    ObjectBoolean &operator=(const ObjectBoolean &);
};

class ObjectNumber: public Object {
public:
    explicit ObjectNumber(Number n): n(n) {}
    virtual bool getNumber(Number &r) const override { r = n; return true; }
    virtual utf8string toString() const override { return utf8string(number_to_string(n)); }
private:
    const Number n;
private:
    ObjectNumber(const ObjectNumber &);
    ObjectNumber &operator=(const ObjectNumber &);
};

class ObjectString: public Object {
public:
    explicit ObjectString(const utf8string &s): s(s) {}
    virtual bool getString(utf8string &r) const override { r = s; return true; }
    virtual utf8string toLiteralString() const override;
    virtual utf8string toString() const override { return s; }
private:
    const utf8string s;
private:
    ObjectString(const ObjectString &);
    ObjectString &operator=(const ObjectString &);
};

class ObjectBytes: public Object {
public:
    explicit ObjectBytes(const std::vector<unsigned char> &b): b(b) {}
    virtual bool getBytes(std::vector<unsigned char> &r) const override { r = b; return true; }
    virtual utf8string toString() const override {
        utf8string r {"HEXBYTES \""};
        bool first = true;
        for (auto x: b) {
            if (first) {
                first = false;
            } else {
                r += ' ';
            }
            char buf[3];
            snprintf(buf, sizeof(buf), "%02x", x);
            r += buf;
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

class ObjectArray: public Object {
public:
    explicit ObjectArray(const std::vector<std::shared_ptr<Object>> &a): a(a) {}
    virtual bool getArray(std::vector<std::shared_ptr<Object>> &r) const override { r = a; return true; }
    virtual bool subscript(std::shared_ptr<Object> index, std::shared_ptr<Object> &r) const override;
    virtual utf8string toString() const override;
private:
    std::vector<std::shared_ptr<Object>> a;
private:
    ObjectArray(const ObjectArray &);
    ObjectArray &operator=(const ObjectArray &);
};

class ObjectDictionary: public Object {
public:
    explicit ObjectDictionary(const std::map<utf8string, std::shared_ptr<Object>> &d): d(d) {}
    virtual bool getDictionary(std::map<utf8string, std::shared_ptr<Object>> &r) const override { r = d; return true; }
    virtual bool subscript(std::shared_ptr<Object> index, std::shared_ptr<Object> &r) const override;
    virtual utf8string toString() const override;
private:
    std::map<utf8string, std::shared_ptr<Object>> d;
private:
    ObjectDictionary(const ObjectDictionary &);
    ObjectDictionary &operator=(const ObjectDictionary &);
};

#endif // OBJECT_H
