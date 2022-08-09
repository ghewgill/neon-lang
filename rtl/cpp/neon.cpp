#include "neon.h"

#include <iostream>

namespace neon {

namespace builtin {

std::shared_ptr<Object> object__makeString(const std::string &s)
{
    return std::make_shared<StringObject>(s);
}

} // namespace builtin

namespace global {

void print(std::shared_ptr<Object> x)
{
    std::cout << x->toString() << "\n";
}

} // namespace global

} // namespace neon
