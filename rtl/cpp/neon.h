#include <iostream>
#include <memory>
#include <string>

namespace neon {

class Object {
public:
    virtual std::string toString() = 0;
};

class StringObject: public Object {
public:
    std::string s;
public:
    explicit StringObject(const std::string &s): s(s) {}
    virtual std::string toString() override { return s; }
};

namespace global {

std::shared_ptr<Object> object__makeString(const std::string &s);
void print(std::shared_ptr<Object> x);

} // namespace global

class NeonException {
public:
    std::string name;
};

} // namespace neon
