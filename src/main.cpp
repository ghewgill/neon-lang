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

int main(int argc, char *argv[])
{
    bool dump_tokens = false;
    bool dump_parse = false;
    bool dump_ast = false;
    bool dump_listing = false;
    bool enable_assert = true;
    unsigned short debug_port = 0;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s filename.neon\n", argv[0]);
        exit(1);
    }

    int a = 1;
    while (a < argc && argv[a][0] == '-' && argv[a][1] != '\0') {
        std::string arg = argv[a];
        if (arg == "-c") {
            if (argv[a+1] == NULL) {
                fprintf(stderr, "%s: -c requires argument\n", argv[0]);
                exit(1);
            }
            break;
        } else if (arg == "-d") {
            a++;
            debug_port = static_cast<unsigned short>(std::stoul(argv[a]));
        } else if (arg == "-l") {
            dump_listing = true;
        } else if (arg == "-n") {
            enable_assert = false;
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
    } else if (name == "-c") {
        buf << argv[a+1];
    } else {
        auto i = name.find_last_of("/:\\");
        if (i != std::string::npos) {
            source_path = name.substr(0, i+1);
        }
        std::ifstream inf(name);
        if (not inf) {
            fprintf(stderr, "Source file not found: %s\n", name.c_str());
            exit(1);
        }
        buf << inf.rdbuf();
    }

    CompilerSupport compiler_support(source_path);
    RuntimeSupport runtime_support(source_path);

    std::vector<unsigned char> bytecode;
    // TODO - Allow reading debug information from file.
    DebugInfo debug(name, buf.str());

    // Pretty hacky way of checking whether the input file is compiled or not.
    if (name[name.length()-1] != 'x') {

        try {
            auto tokens = tokenize(name, buf.str());
            if (dump_tokens) {
                dump(*tokens);
            }

            auto parsetree = parse(*tokens);
            if (dump_parse) {
                dump(parsetree.get());
            }

            auto ast = analyze(&compiler_support, parsetree.get());
            if (dump_ast) {
                dump(ast);
            }

            bytecode = compile(ast, &debug);
            if (dump_listing) {
                disassemble(bytecode, std::cerr, &debug);
            }

        } catch (CompilerError *error) {
            error->write(std::cerr);
            exit(1);
        }

    } else {
        std::string s = buf.str();
        std::copy(s.begin(), s.end(), std::back_inserter(bytecode));
    }

    exec(name, bytecode, &debug, &runtime_support, enable_assert, debug_port, argc-a, argv+a);
}
