#include <iostream>
#include <string>

namespace neon {

namespace global {

void print(const std::string &s);

} // namespace global

class NeonException {
public:
    std::string name;
};

} // namespace neon
