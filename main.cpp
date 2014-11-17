#include <fstream>
#include <iostream>
#include <sstream>

#include "ast.h"
#include "compiler.h"
#include "disassembler.h"
#include "exec.h"
#include "lexer.h"
#include "parser.h"

static std::vector<Token> dump(const std::vector<Token> &tokens)
{
    for (auto t: tokens) {
        std::cerr << t.tostring() << "\n";
    }
    return tokens;
}

static const Program *dump(const Program *program)
{
    program->dump(std::cerr);
    return program;
}

int main(int argc, char *argv[])
{
    bool dump_tokens = false;
    bool dump_ast = false;
    bool dump_bytecode = false;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s filename.simple\n", argv[0]);
        exit(1);
    }

    std::ifstream inf(argv[1]);
    std::stringstream buf;
    buf << inf.rdbuf();

    auto tokens = tokenize(buf.str());
    if (dump_tokens) {
        dump(tokens);
    }

    auto ast = parse(tokens);
    if (dump_ast) {
        dump(ast);
    }

    auto bytecode = compile(ast);
    if (dump_bytecode) {
        disassemble(bytecode, std::cerr);
    }

    exec(bytecode);
}
