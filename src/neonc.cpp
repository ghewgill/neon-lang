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

int main(int argc, char *argv[])
{
    bool ignore_errors = false;
    bool listing = false;

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

        std::stringstream buf;
        std::string name = argv[a];
        if (name == "-") {
            std::cout << "Compiling stdin (no output file)...\n";
            buf << std::cin.rdbuf();
        } else {
            std::cout << "Compiling " << name << "...\n";
            std::ifstream inf(name);
            buf << inf.rdbuf();
        }

        try {
            auto tokens = tokenize(buf.str());
            auto parsetree = parse(tokens);
            auto ast = analyze(parsetree);
            DebugInfo debug(buf.str());
            auto bytecode = compile(ast, &debug);
            if (listing) {
                disassemble(bytecode, std::cerr, &debug);
            }

            if (name != "-") {
                std::ofstream outf(std::string(argv[a]) + "x", std::ios::binary);
                outf.write(reinterpret_cast<const std::ofstream::char_type *>(bytecode.data()), bytecode.size());
            }

        } catch (CompilerError &error) {
            error.write(std::cerr);
            if (not ignore_errors) {
                exit(1);
            }
        }

        a++;
    }
}
