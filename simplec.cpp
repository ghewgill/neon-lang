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

int main(int argc, char *argv[])
{
    bool ignore_errors = false;
    bool listing = false;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s filename.simple\n", argv[0]);
        exit(1);
    }

    int a = 1;
    while (a < argc) {
        if (std::string(argv[a]) == "-i") {
            ignore_errors = true;
            a++;
            continue;
        }

        std::cout << "Compiling " << argv[a] << "...\n";
        std::ifstream inf(argv[a]);
        std::stringstream buf;
        buf << inf.rdbuf();

        try {
            auto tokens = tokenize(buf.str());
            auto ast = parse(tokens);
            DebugInfo debug(buf.str());
            auto bytecode = compile(ast, &debug);
            if (listing) {
                disassemble(bytecode, std::cerr, &debug);
            }

            std::ofstream outf(std::string(argv[1]) + "x");
            outf.write(reinterpret_cast<const std::ofstream::char_type *>(bytecode.data()), bytecode.size());

        } catch (SourceError &error) {
            fprintf(stderr, "%s\n", error.token.source.c_str());
            fprintf(stderr, "%*s\n", error.token.column, "^");
            fprintf(stderr, "Error S%d: %d:%d %s %s (%s:%d)\n", error.number, error.token.line, error.token.column, error.token.tostring().c_str(), error.message.c_str(), error.file.c_str(), error.line);
            if (not ignore_errors) {
                exit(1);
            }
        } catch (InternalError &error) {
            fprintf(stderr, "Compiler Internal Error: %s (%s:%d)\n", error.message.c_str(), error.file.c_str(), error.line);
            if (not ignore_errors) {
                exit(1);
            }
        }

        a++;
    }
}
