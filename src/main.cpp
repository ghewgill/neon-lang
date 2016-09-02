#include <fstream>
#include <iostream>
#include <sstream>

#include "analyzer.h"
#include "ast.h"
#include "cell.h"
#include "compiler.h"
#include "debuginfo.h"
#include "disassembler.h"
#include "exec.h"
#include "lexer.h"
#include "parser.h"
#include "pt_dump.h"
#include "support.h"

bool dump_tokens = false;
bool dump_parse = false;
bool dump_ast = false;
bool dump_listing = false;
bool enable_assert = true;
unsigned short debug_port = 0;
bool repl_no_prompt = false;
bool repl_stop_on_any_error = false;

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
    if (not repl_no_prompt) {
        std::cout << "Neon 0.1\n";
        std::cout << "Type \"help\" for more information, or \"exit\" to leave.\n";
    }
    std::map<std::string, ExternalGlobalInfo> globals_ast;
    std::map<std::string, Cell *> globals_cells;
    std::vector<std::unique_ptr<TokenizedSource>> input;
    for (;;) {
        if (not repl_no_prompt) {
            std::cout << "> ";
        }
        std::string s;
        if (not std::getline(std::cin, s)) {
            if (not repl_no_prompt) {
                std::cout << std::endl;
            }
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
                auto tokens = tokenize("", s);
                const Program *ast;
                try {
                    auto parsetree = parse(*tokens);
                    // Grab a copy of the globals and let analyze() modify the copy.
                    // Some errors will be detected after things are added to the
                    // global scope, and we don't want to capture those global
                    // things if there was an error. (Errors are handled by exception
                    // so the assignment back to globals_ast won't happen.)
                    auto globals = globals_ast;
                    ast = analyze(&compiler_support, parsetree.get(), &globals);
                    globals_ast = globals;
                    for (auto g: globals_ast) {
                        if (globals_cells.find(g.first) == globals_cells.end()) {
                            globals_cells[g.first] = new Cell();
                        }
                    }
                } catch (CompilerError *) {
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
                            )}
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
                    ast = analyze(&compiler_support, parsetree.get(), &globals_ast);
                }
                DebugInfo debug("-", s);
                auto bytecode = compile(ast, &debug);
                if (dump_listing) {
                    disassemble(bytecode, std::cerr, &debug);
                }
                exec("-", bytecode, &debug, &runtime_support, false, 0, argc, argv, &globals_cells);
                input.emplace_back(std::move(tokens));
            } catch (CompilerError *error) {
                error->write(std::cerr);
                if (repl_stop_on_any_error) {
                    exit(1);
                }
            }
        }
    }
    exit(0);
}

int main(int argc, char *argv[])
{
    if (argc < 1) {
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
        } else if (arg == "--repl-no-prompt") {
            repl_no_prompt = true;
        } else if (arg == "--repl-stop-on-any-error") {
            repl_stop_on_any_error = true;
        } else {
            fprintf(stderr, "Unknown option: %s\n", arg.c_str());
            exit(1);
        }
        a++;
    }

    if (a >= argc) {
        repl(argc, argv);
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
