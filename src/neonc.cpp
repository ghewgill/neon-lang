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

extern void compile_cli(CompilerSupport *support, const ast::Program *);
extern void compile_cpp(CompilerSupport *support, const ast::Program *);
extern void compile_js(CompilerSupport *support, const ast::Program *);
extern void compile_jvm(CompilerSupport *support, const ast::Program *);

struct {
    std::string name;
    CompileProc proc;
} Targets[] = {
    {"cli", compile_cli},
    {"cpp", compile_cpp},
    {"js", compile_js},
    {"jvm", compile_jvm},
};

int main(int argc, char *argv[])
{
    bool ignore_errors = false;
    bool listing = false;
    std::string output;
    bool quiet = false;
    bool error_json = false;
    std::string target;
    CompileProc target_proc = nullptr;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s filename.neon\n", argv[0]);
        exit(1);
    }

    int a = 1;
    while (a < argc && argv[a][0] == '-' && argv[a][1] != '\0') {
        std::string arg = argv[a];
        if (arg == "-i") {
            ignore_errors = true;
        } else if (arg == "-d") {
            listing = true;
        } else if (arg == "--json") {
            error_json = true;
        } else if (arg == "-o") {
            a++;
            if (a >= argc) {
                std::cerr << "Missing output name\n";
                exit(1);
            }
            output = argv[a];
        } else if (arg == "-q") {
            quiet = true;
        } else if (arg == "-t") {
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
        } else {
            fprintf(stderr, "Unknown option: %s\n", arg.c_str());
            exit(1);
        }
        a++;
    }

    while (a < argc) {
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

        CompilerSupport compiler_support(source_path, target_proc);

        const std::string objname = output.empty() ? std::string(argv[a]) + "x" : output;
        if (target_proc == nullptr) {
            remove(objname.c_str());
        }
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
                    std::ofstream outf(objname, std::ios::binary);
                    outf.write(reinterpret_cast<const std::ofstream::char_type *>(bytecode.data()), bytecode.size());
                }
            } else {
                target_proc(&compiler_support, ast);
            }

        } catch (CompilerError *error) {
            if (error_json) {
                error->write_json(std::cerr);
            } else {
                error->write(std::cerr);
            }
            if (not ignore_errors) {
                exit(1);
            }
        }

        a++;
    }
}
