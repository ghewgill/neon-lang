#include <fstream>
#include <iostream>
#include <sstream>

#include "analyzer.h"
#include "ast.h"
#include "compiler.h"
#include "debuginfo.h"
#include "disassembler.h"
#include "lexer.h"
#include "parser.h"
#include "pt_dump.h"
#include "support.h"

typedef void (*TargetProc)(const ast::Program *);

extern void compile_jvm(const ast::Program *);

struct {
    std::string name;
    TargetProc proc;
} Targets[] = {
    {"jvm", compile_jvm},
};

int main(int argc, char *argv[])
{
    bool ignore_errors = false;
    bool listing = false;
    bool quiet = false;
    std::string target;
    TargetProc target_proc = nullptr;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s filename.neon\n", argv[0]);
        exit(1);
    }

    int a = 1;
    while (a < argc) {
        if (std::string(argv[a]) == "-i") {
            ignore_errors = true;
            a++;
            continue;
        }
        if (std::string(argv[a]) == "-d") {
            listing = true;
            a++;
            continue;
        }
        if (std::string(argv[a]) == "-q") {
            quiet = true;
            a++;
            continue;
        }
        if (std::string(argv[a]) == "-t") {
            a++;
            if (a < argc) {
                target = argv[a];
                size_t i = 0;
                while (i < sizeof(Targets)/sizeof(Targets[0]) && target != Targets[i].name) {
                    i++;
                }
                if (i < sizeof(Targets)/sizeof(Targets[0])) {
                    target_proc = Targets[i].proc;
                } else {
                    std::cerr << "Unknown target: " << target << "\n";
                    exit(1);
                }
            }
            a++;
            continue;
        }

        std::string name = argv[a];
        std::string source_path;

        std::stringstream buf;
        if (name == "-") {
            if (not quiet) {
                std::cout << "Compiling stdin (no output file)";
            }
            buf << std::cin.rdbuf();
        } else {
            auto i = name.find_last_of("/:\\");
            if (i != std::string::npos) {
                source_path = name.substr(0, i+1);
            }
            if (not quiet) {
                std::cout << "Compiling " << name;
            }
            std::ifstream inf(name);
            buf << inf.rdbuf();
        }
        if (not quiet) {
            if (not target.empty()) {
                std::cout << " for target " << target;
            }
            std::cout << "...\n";
        }

        CompilerSupport compiler_support(source_path);

        try {
            auto tokens = tokenize(name, buf.str());
            auto parsetree = parse(*tokens);
            auto ast = analyze(&compiler_support, parsetree.get());
            DebugInfo debug(name, buf.str());
            if (target_proc == nullptr) {
                auto bytecode = compile(ast, &debug);
                if (listing) {
                    disassemble(bytecode, std::cerr, &debug);
                }
                if (name != "-") {
                    std::ofstream outf(std::string(argv[a]) + "x", std::ios::binary);
                    outf.write(reinterpret_cast<const std::ofstream::char_type *>(bytecode.data()), bytecode.size());
                }
            } else {
                target_proc(ast);
            }

        } catch (CompilerError *error) {
            error->write(std::cerr);
            if (not ignore_errors) {
                exit(1);
            }
        }

        a++;
    }
}
