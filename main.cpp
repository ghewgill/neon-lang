#include <fstream>
#include <iostream>
#include <sstream>

#include "ast.h"
#include "compiler.h"
#include "debuginfo.h"
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

    try {
        auto tokens = tokenize(buf.str());
        if (dump_tokens) {
            dump(tokens);
        }

        auto ast = parse(tokens);
        if (dump_ast) {
            dump(ast);
        }

        DebugInfo debug(buf.str());
        auto bytecode = compile(ast, &debug);
        if (dump_bytecode) {
            disassemble(bytecode, std::cerr, &debug);
        }

        exec(bytecode);

    } catch (SourceError &error) {
        fprintf(stderr, "%s\n", error.token.source.c_str());
        fprintf(stderr, "%*s\n", error.token.column, "^");
        fprintf(stderr, "Error S%d: %d:%d %s %s (%s:%d)\n", error.number, error.token.line, error.token.column, error.token.tostring().c_str(), error.message.c_str(), error.file.c_str(), error.line);
        exit(1);
    } catch (InternalError &error) {
        fprintf(stderr, "Compiler Internal Error: %s (%s:%d)\n", error.message.c_str(), error.file.c_str(), error.line);
    }
}
