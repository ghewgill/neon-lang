#include "support.h"

#include <fstream>
#include <iso646.h>
#include <sstream>

#include <sha256.h>

#include "analyzer.h"
#include "ast.h"
#include "lexer.h"
#include "parser.h"
#include "compiler.h"

#ifdef USE_RTLX
#include "rtl.inc"
#endif

void CompilerSupport::loadBytecode(const std::string &name, Bytecode &object)
{
#ifdef USE_RTLX
    for (size_t i = 0; i < sizeof(rtl_sources)/sizeof(rtl_sources[0]); i++) {
        if (name == rtl_sources[i].name) {
            auto tokens = tokenize(name, rtl_sources[i].source);
            auto parsetree = parse(*tokens);
            auto ast = analyze(this, parsetree.get());
            auto bytecode = compile(ast, nullptr);
            object.load("-builtin-", bytecode);
            return;
        }
    }
#endif

    std::pair<std::string, std::string> names = findModule(name);
    if (names.first.empty() && names.second.empty()) {
        throw BytecodeException("file not found");
    }

    std::ifstream obj_file(names.second, std::ios::binary);
    std::ifstream src_file(names.first);

    std::string source_text;
    if (src_file.good()) {
        std::stringstream buf;
        buf << src_file.rdbuf();
        source_text = buf.str();
    }

    if (not source_text.empty()) {
        if (obj_file.good()) {
            SHA256 sha256;
            sha256(source_text);
            unsigned char h[SHA256::HashBytes];
            sha256.getHash(h);
            std::string hash = std::string(h, h+sizeof(h));

            std::stringstream buf;
            buf << obj_file.rdbuf();
            std::vector<unsigned char> bytecode;
            std::string s = buf.str();
            std::copy(s.begin(), s.end(), std::back_inserter(bytecode));
            object.load(name, bytecode);

            if (object.source_hash == hash) {
                return;
            }
            object = Bytecode();
        }

        obj_file.close();
        const std::string objname = names.first + "x";
        remove(objname.c_str());
        {
            auto tokens = tokenize(names.first, source_text);
            auto parsetree = parse(*tokens);
            auto ast = analyze(this, parsetree.get());
            auto bytecode = compile(ast, nullptr);
            writeOutput(objname, bytecode);
            if (cproc != nullptr) {
                cproc(this, ast, "", {});
            }
        }
        obj_file.open(objname, std::ios::binary);
        if (not obj_file.good()) {
            throw BytecodeException("file not found");
        }
    }

    std::stringstream buf;
    buf << obj_file.rdbuf();
    std::vector<unsigned char> bytecode;
    std::string s = buf.str();
    std::copy(s.begin(), s.end(), std::back_inserter(bytecode));
    object.load(names.first.empty() ? names.second : names.first, bytecode);
}

void CompilerSupport::writeOutput(const std::string &name, const std::vector<unsigned char> &content)
{
    std::ofstream f(name, std::ios::binary);
    f.write(reinterpret_cast<const char *>(content.data()), content.size());
}
