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

extern void compile_cli(CompilerSupport *support, const ast::Program *, std::string output, std::map<std::string, std::string> options);
extern void compile_cpp(CompilerSupport *support, const ast::Program *, std::string output, std::map<std::string, std::string> options);
extern void compile_js(CompilerSupport *support, const ast::Program *, std::string output, std::map<std::string, std::string> options);
extern void compile_jvm(CompilerSupport *support, const ast::Program *, std::string output, std::map<std::string, std::string> options);

struct {
    std::string name;
    CompileProc proc;
    std::string description;
} Targets[] = {
    {"cli", compile_cli, "CLI (.NET) target. Output file name is a .exe file."},
    {"cpp", compile_cpp, "C++ target. Output file name is a .cpp source file."},
    {"js",  compile_js,  "Javascript target. Output file name is a .js source file."},
    {"jvm", compile_jvm, "JVM (Java VM) target. Output file is a .class file."},
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
    std::map<std::string, std::string> options;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s [options] filename.neon ...\n", argv[0]);
        fprintf(stderr, "\n");
        fprintf(stderr, "    Options:\n");
        fprintf(stderr, "        -i          Ignore any errors, compile all named source files\n");
        fprintf(stderr, "        -d          Print disassembly listing\n");
        fprintf(stderr, "        --json      Print error messages in JSON form\n");
        fprintf(stderr, "        --neonpath  Append given path to library search path\n");
        fprintf(stderr, "        -o filename Output file name\n");
        fprintf(stderr, "        -q          Quiet, print messages only in case of errors\n");
        fprintf(stderr, "        -t target   Compilation target, see list below\n");
        fprintf(stderr, "        -T name=val Target specific option setting\n");
        fprintf(stderr, "\n");
        fprintf(stderr, "    Supported targets:\n");
        for (auto &t: Targets) {
            fprintf(stderr, "        %-8s%s\n", t.name.c_str(), t.description.c_str());
        }
        exit(1);
    }

    std::vector<std::string> neonpath;
    int a = 1;
    while (a < argc && argv[a][0] == '-' && argv[a][1] != '\0') {
        std::string arg = argv[a];
        if (arg == "-i") {
            ignore_errors = true;
        } else if (arg == "-d") {
            listing = true;
        } else if (arg == "--json") {
            error_json = true;
        } else if (arg == "--neonpath") {
            a++;
            if (a >= argc) {
                std::cerr << "--neonpath option requires path name\n";
                exit(1);
            }
            neonpath.push_back(argv[a]);
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
        } else if (arg == "-T") {
            a++;
            if (a >= argc) {
                std::cerr << "Missing target option\n";
                exit(1);
            }
            const char *eq = strchr(argv[a], '=');
            if (eq != nullptr) {
                options[std::string(argv[a], eq-argv[a])] = std::string(eq+1);
            } else {
                options[argv[a]] = "";
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
            std::ifstream inf(name);
            if (not inf) {
                std::cerr << "Source file not found: " << name << "\n";
                exit(1);
            }
            buf << inf.rdbuf();
            if (not quiet) {
                std::cout << "Compiling " << name;
            }
        }
        if (not quiet) {
            if (not target.empty()) {
                std::cout << " for target " << target;
            }
            std::cout << "...\n";
        }

        CompilerSupport compiler_support(source_path, neonpath, target_proc);

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
                    if (not outf) {
                        std::cerr << "Could not create output file: " << objname << "\n";
                        exit(1);
                    }
                    outf.write(reinterpret_cast<const std::ofstream::char_type *>(bytecode.data()), bytecode.size());
                }
            } else {
                target_proc(&compiler_support, ast, output, options);
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
