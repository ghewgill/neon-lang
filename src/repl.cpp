#include "repl.h"

#include <iostream>

#include "analyzer.h"
#include "cell.h"
#include "compiler.h"
#include "debuginfo.h"
#include "disassembler.h"
#include "exec.h"
#include "lexer.h"
#include "parser.h"

Repl::Repl(int argc, char *argv[], bool no_prompt, bool stop_on_any_error, bool dump_listing)
  : argc(argc),
    argv(argv),
    stop_on_any_error(stop_on_any_error),
    dump_listing(dump_listing),
    compiler_support("", nullptr),
    runtime_support(""),
    globals_ast(),
    globals_cells(),
    input()
{
    if (not no_prompt) {
        std::cout << "Neon 0.1\n";
        std::cout << "Type \"help\" for more information, or \"exit\" to leave.\n";
    }
}

void Repl::handle(const std::string &s)
{
    if (s == "help") {
        std::cout << "\n";
        std::cout << "Welcome to Neon 0.1!\n";
        std::cout << "\n";
        std::cout << "See https://github.com/ghewgill/neon-lang for information.\n";
    } else if (s == "exit" || s == "quit") {
        exit(0);
    } else {
        CompilerError *first_error = nullptr;
        try {
            auto tokens = tokenize("", s);
            const ast::Program *program;
            try {
                auto parsetree = parse(*tokens);
                // Grab a copy of the globals and let analyze() modify the copy.
                // Some errors will be detected after things are added to the
                // global scope, and we don't want to capture those global
                // things if there was an error. (Errors are handled by exception
                // so the assignment back to globals_ast won't happen.)
                auto globals = globals_ast;
                program = analyze(&compiler_support, parsetree.get(), &globals);
                globals_ast = globals;
                for (auto g: globals_ast) {
                    if (globals_cells.find(g.first) == globals_cells.end()) {
                        globals_cells[g.first] = new Cell();
                    }
                }
            } catch (CompilerError *e) {
                first_error = e;
                auto exprtree = parseExpression(*tokens);
                std::vector<std::unique_ptr<pt::FunctionCallExpression::Argument>> print_args;
                print_args.emplace_back(
                    std::unique_ptr<pt::FunctionCallExpression::Argument> { new pt::FunctionCallExpression::Argument(
                        Token(IN, "IN"),
                        Token(),
                        std::unique_ptr<pt::Expression> { new pt::FunctionCallExpression(
                            Token(),
                            std::unique_ptr<pt::DotExpression> { new pt::DotExpression(
                                Token(),
                                std::move(exprtree),
                                Token(IDENTIFIER, "toString")
                            )},
                            {},
                            Token()
                        )},
                        false
                    )}
                );
                std::vector<std::unique_ptr<pt::Statement>> statements;
                statements.emplace_back(
                    std::unique_ptr<pt::Statement> { new pt::ExpressionStatement(
                        Token(),
                        std::unique_ptr<pt::Expression> { new pt::FunctionCallExpression(
                            Token(),
                            std::unique_ptr<pt::Expression> { new pt::IdentifierExpression(
                                Token(),
                                "print"
                            )},
                            std::move(print_args),
                            Token()
                        )}
                    )}
                );
                auto parsetree = std::unique_ptr<pt::Program> { new pt::Program(
                    Token(),
                    std::move(statements),
                    "",
                    "00000000000000000000000000000000"
                )};
                program = analyze(&compiler_support, parsetree.get(), &globals_ast);
            }
            DebugInfo debug("-", s);
            auto bytecode = compile(program, &debug);
            if (dump_listing) {
                disassemble(bytecode, std::cerr, &debug);
            }
            struct ExecOptions options;
            options.enable_assert = true;
            options.enable_trace = false;
            int r = exec("-", bytecode, &debug, &runtime_support, &options, 0, argc, argv, &globals_cells);
            if (r != 0) {
                fprintf(stderr, "exit code %d\n", r);
            }
            input.emplace_back(std::move(tokens));
        } catch (CompilerError *error) {
            SourceError *se = dynamic_cast<SourceError *>(error);
            // Error 2015 is "Expression expected", which means the real error
            // was more likely to be the first one reported.
            if (se != nullptr && se->number == 2015) {
                first_error->write(std::cerr);
            } else {
                error->write(std::cerr);
            }
            if (stop_on_any_error) {
                exit(1);
            }
        }
    }
}
