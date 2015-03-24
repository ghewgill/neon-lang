#include <fstream>
#include <iostream>
#include <sstream>

#include "analyzer.h"
#include "ast.h"
#include "compiler.h"
#include "debuginfo.h"
#include "disassembler.h"
#include "exec.h"
#include "lexer.h"
#include "parser.h"
#include "pt_dump.h"
#include "support.h"

static TokenizedSource dump(const TokenizedSource &tokens)
{
    for (auto t: tokens.tokens) {
        std::cerr << t.tostring() << "\n";
    }
    return tokens;
}

static const pt::Program *dump(const pt::Program *parsetree)
{
    pt::dump(std::cerr, parsetree);
    return parsetree;
}

static const Program *dump(const Program *program)
{
    program->dump(std::cerr);
    return program;
}

static void repl(int argc, char *argv[])
{
    CompilerSupport compiler_support("");
    RuntimeSupport runtime_support("");
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
                auto parsetree = parse(tokenize(s));
                auto ast = analyze(&compiler_support, parsetree);
                DebugInfo debug(s);
                auto bytecode = compile(ast, &debug);
                exec(bytecode, &debug, &runtime_support, argc, argv);
            } catch (CompilerError &error) {
                error.write(std::cerr);
            }
        }
    }
    exit(0);
}

int main(int argc, char *argv[])
{
    bool dump_tokens = false;
    bool dump_parse = false;
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
    std::string source_path;

    std::stringstream buf;
    if (name == "-") {
        buf << std::cin.rdbuf();
    } else {
        auto i = name.find_last_of("/:\\");
        if (i != std::string::npos) {
            source_path = name.substr(0, i+1);
        }
        std::ifstream inf(name);
        buf << inf.rdbuf();
    }

    CompilerSupport compiler_support(source_path);
    RuntimeSupport runtime_support(source_path);

    std::vector<unsigned char> bytecode;
    // TODO - Allow reading debug information from file.
    DebugInfo debug(buf.str());

    // Pretty hacky way of checking whether the input file is compiled or not.
    if (name[name.length()-1] != 'x') {

        try {
            auto tokens = tokenize(buf.str());
            if (dump_tokens) {
                dump(tokens);
            }

            auto parsetree = parse(tokens);
            if (dump_parse) {
                dump(parsetree);
            }

            auto ast = analyze(&compiler_support, parsetree);
            if (dump_ast) {
                dump(ast);
            }

            bytecode = compile(ast, &debug);
            if (dump_bytecode) {
                disassemble(bytecode, std::cerr, &debug);
            }

        } catch (CompilerError &error) {
            error.write(std::cerr);
            exit(1);
        }

    } else {
        std::string s = buf.str();
        std::copy(s.begin(), s.end(), std::back_inserter(bytecode));
    }

    exec(bytecode, &debug, &runtime_support, argc-a, argv+a);
}
