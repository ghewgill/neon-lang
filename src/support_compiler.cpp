#include "support.h"

#include <fstream>
#include <iso646.h>
#include <sstream>

#include <sha256.h>

#include "lexer.h"
#include "parser.h"
#include "compiler.h"

bool CompilerSupport::loadBytecode(const std::string &name, Bytecode &object)
{
    std::pair<std::string, std::string> names = findModule(name);
    std::ifstream obj(names.second, std::ios::binary);
    std::ifstream src(names.first);

    std::string source;
    if (src.good()) {
        std::stringstream buf;
        buf << src.rdbuf();
        source = buf.str();
    }

    if (obj.good() && not source.empty()) {
        SHA256 sha256;
        sha256(source);
        unsigned char h[SHA256::HashBytes];
        sha256.getHash(h);
        std::string hash = std::string(h, h+sizeof(h));

        std::stringstream buf;
        buf << obj.rdbuf();
        std::vector<unsigned char> bytecode;
        std::string s = buf.str();
        std::copy(s.begin(), s.end(), std::back_inserter(bytecode));
        object = Bytecode(bytecode);

        if (object.source_hash == hash) {
            return true;
        }
    }

    {
        auto tokens = tokenize(source);
        auto parsetree = parse(tokens);
        auto ast = analyze(this, parsetree);
        auto bytecode = compile(ast, nullptr);
        std::ofstream outf(names.first+"x", std::ios::binary);
        outf.write(reinterpret_cast<const std::ofstream::char_type *>(bytecode.data()), bytecode.size());
        obj.open(names.first+"x", std::ios::binary);
        if (not obj.good()) {
            return false;
        }
    }

    std::stringstream buf;
    buf << obj.rdbuf();
    std::vector<unsigned char> bytecode;
    std::string s = buf.str();
    std::copy(s.begin(), s.end(), std::back_inserter(bytecode));
    object = Bytecode(bytecode);
    return true;
}
