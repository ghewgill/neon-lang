#include "support.h"

#include <fstream>
#include <sstream>

#include "lexer.h"
#include "parser.h"
#include "compiler.h"

bool CompilerSupport::loadBytecode(const std::string &name, Bytecode &object)
{
    std::ifstream inf("t/"+name+".neonx", std::ios::binary);
    if (not inf.good()) {
        std::ifstream src("t/"+name+".neon");
        std::stringstream buf;
        buf << src.rdbuf();
        auto tokens = tokenize(buf.str());
        auto parsetree = parse(tokens);
        auto ast = analyze(this, parsetree);
        auto bytecode = compile(ast, nullptr);
        std::ofstream outf("t/"+name+".neonx", std::ios::binary);
        outf.write(reinterpret_cast<const std::ofstream::char_type *>(bytecode.data()), bytecode.size());
        inf.open("t/"+name+".neonx", std::ios::binary);
        if (not inf.good()) {
            return false;
        }
    }
    std::stringstream buf;
    buf << inf.rdbuf();
    std::vector<unsigned char> bytecode;
    std::string s = buf.str();
    std::copy(s.begin(), s.end(), std::back_inserter(bytecode));
    object = Bytecode(bytecode);
    return true;
}

CompilerSupport g_compiler_support;
