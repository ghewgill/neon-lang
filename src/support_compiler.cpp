#include "support.h"

#include <fstream>
#include <iso646.h>
#include <sstream>

#include <sha256.h>

#include "ast.h"
#include "lexer.h"
#include "parser.h"
#include "compiler.h"

bool CompilerSupport::loadBytecode(const std::string &name, Bytecode &object)
{
    std::pair<std::string, std::string> names = findModule(name);
    if (names.first.empty() && names.second.empty()) {
        return false;
    }

    std::ifstream obj(names.second, std::ios::binary);
    std::ifstream src(names.first);

    std::string source;
    if (src.good()) {
        std::stringstream buf;
        buf << src.rdbuf();
        source = buf.str();
    }

    // TODO: skipping the following check causes modules to always be rebuilt
    //if (obj.good() && not source.empty()) {
    //    SHA256 sha256;
    //    sha256(source);
    //    unsigned char h[SHA256::HashBytes];
    //    sha256.getHash(h);
    //    std::string hash = std::string(h, h+sizeof(h));
    //
    //    std::stringstream buf;
    //    buf << obj.rdbuf();
    //    std::vector<unsigned char> bytecode;
    //    std::string s = buf.str();
    //    std::copy(s.begin(), s.end(), std::back_inserter(bytecode));
    //    object = Bytecode(bytecode);
    //
    //    if (object.source_hash == hash) {
    //        return true;
    //    }
    //}

    if (not source.empty()) {
        obj.close();
        const std::string objname = names.first + "x";
        remove(objname.c_str());
        {
            auto tokens = tokenize(names.first, source);
            auto parsetree = parse(tokens);
            auto ast = analyze(this, parsetree.get());
            auto bytecode = compile(ast, nullptr);
            std::ofstream outf(objname, std::ios::binary);
            outf.write(reinterpret_cast<const std::ofstream::char_type *>(bytecode.data()), bytecode.size());
        }
        obj.open(objname, std::ios::binary);
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
