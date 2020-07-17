#include "support.h"

#include <fstream>
#include <iso646.h>
#include <iterator>
#include <sstream>

#include "bytecode.h"

/* USE_RTLX is a way to embed the standard library compiled *.neonx
 * files into the neonx executable. However, doing this in the build
 * first requires building the compiler itself, then running the compiler
 * to build the *.neonx files, before building neonx. This is
 * incompatible with cross-compiled builds so is disabled at this time.
 */
#ifdef USE_RTLX
#include "rtlx.inc"
#endif

void RuntimeSupport::loadBytecode(const std::string &name, Bytecode &object)
{
#ifdef USE_RTLX
    for (size_t i = 0; i < sizeof(rtl_bytecode)/sizeof(rtl_bytecode[0]); i++) {
        if (name == rtl_bytecode[i].name) {
            std::vector<unsigned char> bytecode {rtl_bytecode[i].bytecode, rtl_bytecode[i].bytecode + rtl_bytecode[i].length};
            object.load("-builtin-", bytecode);
            return true;
        }
    }
#endif

    std::pair<std::string, std::string> names = findModule(name);
    std::ifstream inf(names.second, std::ios::binary);
    if (not inf.good()) {
        throw BytecodeException("file not found");
    }
    std::stringstream buf;
    buf << inf.rdbuf();
    std::vector<unsigned char> bytecode;
    std::string s = buf.str();
    std::copy(s.begin(), s.end(), std::back_inserter(bytecode));
    object.load(names.second, bytecode);
}
