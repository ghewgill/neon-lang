#include "support.h"

#include <fstream>
#include <iso646.h>
#include <sstream>

bool RuntimeSupport::loadBytecode(const std::string &name, Bytecode &object)
{
    std::ifstream inf("t/"+name+".neonx", std::ios::binary);
    if (not inf.good()) {
        return false;
    }
    std::stringstream buf;
    buf << inf.rdbuf();
    std::vector<unsigned char> bytecode;
    std::string s = buf.str();
    std::copy(s.begin(), s.end(), std::back_inserter(bytecode));
    object = Bytecode(bytecode);
    return true;
}
