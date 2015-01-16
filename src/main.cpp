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

static void repl(int argc, char *argv[])
{
    std::cout << "Neon 0.1\n";
    std::cout << "Type \"help\" for more information, or \"exit\" to leave.\n";
    for (;;) {
        std::cout << "> ";
        std::string s;
        if (not std::getline(std::cin, s)) {
            std::cout << std::endl;
            break;
        }
        if (s == "help") {
            std::cout << "\n";
            std::cout << "Welcome to Neon 0.1!\n";
            std::cout << "\n";
            std::cout << "See https://github.com/ghewgill/neon-lang for information.\n";
        } else if (s == "exit" || s == "quit") {
            exit(0);
        } else {
            try {
                exec(compile(parse(tokenize(s)), nullptr), argc, argv);
            } catch (SourceError &error) {
                fprintf(stderr, "%s\n", error.token.source.c_str());
                fprintf(stderr, "%*s\n", error.token.column, "^");
                fprintf(stderr, "Error N%d: %d:%d %s %s (%s:%d)\n", error.number, error.token.line, error.token.column, error.token.tostring().c_str(), error.message.c_str(), error.file.c_str(), error.line);
            } catch (InternalError &error) {
                fprintf(stderr, "Compiler Internal Error: %s (%s:%d)\n", error.message.c_str(), error.file.c_str(), error.line);
            }
        }
    }
    exit(0);
}

int main(int argc, char *argv[])
{
    bool dump_tokens = false;
    bool dump_ast = false;
    bool dump_bytecode = false;

    if (argc < 1) {
        fprintf(stderr, "Usage: %s filename.neon\n", argv[0]);
        exit(1);
    }

    if (argc < 2) {
        repl(argc, argv);
    }

    int a = 1;
    while (a < argc && argv[a][0] == '-') {
        std::string arg = argv[a];
        if (arg == "-d") {
            dump_bytecode = true;
        } else {
            fprintf(stderr, "Unknown option: %s\n", arg.c_str());
            exit(1);
        }
        a++;
    }

    const std::string name = argv[a];

    std::stringstream buf;
    if (name == "-") {
        buf << std::cin.rdbuf();
    } else {
        std::ifstream inf(name);
        buf << inf.rdbuf();
    }

    std::vector<unsigned char> bytecode;

    // Pretty hacky way of checking whether the input file is compiled or not.
    if (name[name.length()-1] != 'x') {

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
            bytecode = compile(ast, &debug);
            if (dump_bytecode) {
                disassemble(bytecode, std::cerr, &debug);
            }

        } catch (SourceError &error) {
            fprintf(stderr, "%s\n", error.token.source.c_str());
            fprintf(stderr, "%*s\n", error.token.column, "^");
            fprintf(stderr, "Error N%d: %d:%d %s %s (%s:%d)\n", error.number, error.token.line, error.token.column, error.token.tostring().c_str(), error.message.c_str(), error.file.c_str(), error.line);
            exit(1);
        } catch (InternalError &error) {
            fprintf(stderr, "Compiler Internal Error: %s (%s:%d)\n", error.message.c_str(), error.file.c_str(), error.line);
            exit(1);
        }

    } else {
        std::string s = buf.str();
        std::copy(s.begin(), s.end(), std::back_inserter(bytecode));
    }

    exec(bytecode, argc-a, argv+a);
}
