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
#include "repl.h"
#include "support.h"

bool dump_tokens = false;
bool dump_parse = false;
bool dump_ast = false;
bool dump_listing = false;
bool enable_assert = true;
bool enable_debug = false;
bool enable_trace = false;
bool error_json = false;
unsigned short debug_port = 0;
const char *repl_input = nullptr;
bool repl_no_prompt = false;
bool repl_stop_on_any_error = false;

static bool has_suffix(const std::string &str, const std::string &suffix)
{
    return str.size() >= suffix.size() && str.substr(str.size() - suffix.size()) == suffix;
}

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

static const ast::Program *dump(const ast::Program *program)
{
    program->dump(std::cerr);
    return program;
}

static void repl(int argc, char *argv[], const ExecOptions &options)
{
    std::istream *in = &std::cin;
    std::ifstream repl_in;
    if (repl_input != nullptr) {
        repl_in.open(repl_input);
        if (not repl_in) {
            fprintf(stderr, "%s: could not open --repl-input file: %s\n", argv[0], repl_input);
            exit(1);
        }
        in = &repl_in;
    }
    Repl repl(argc, argv, repl_no_prompt, repl_stop_on_any_error, dump_listing, options);
    for (;;) {
        if (not repl_no_prompt) {
            std::cout << "> ";
        }
        std::string s;
        if (not std::getline(*in, s)) {
            if (not repl_no_prompt) {
                std::cout << std::endl;
            }
            break;
        }
        repl.handle(s);
    }
    exit(0);
}

int main(int argc, char *argv[])
{
    if (argc < 1) {
        fprintf(stderr, "Usage: %s filename.neon\n", argv[0]);
        exit(1);
    }

    std::vector<std::string> neonpath;
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
            enable_debug = true;
        } else if (arg == "--debug-port") {
            a++;
            try {
                debug_port = static_cast<unsigned short>(std::stoul(argv[a]));
            } catch (std::invalid_argument &) {
                fprintf(stderr, "%s: --debug-port requires integer argument\n", argv[0]);
                exit(1);
            }
        } else if (arg == "--json") {
            error_json = true;
        } else if (arg == "-l") {
            dump_listing = true;
        } else if (arg == "-n") {
            enable_assert = false;
        } else if (arg == "--neonpath") {
            a++;
            if (argv[a] == NULL) {
                fprintf(stderr, "%s: --neonpath requires path name\n", argv[0]);
                exit(1);
            }
            neonpath.push_back(argv[a]);
        } else if (arg == "--repl-input") {
            a++;
            if (argv[a] == NULL) {
                fprintf(stderr, "%s: --repl-input requires filename\n", argv[0]);
                exit(1);
            }
            repl_input = argv[a];
        } else if (arg == "--repl-no-prompt") {
            repl_no_prompt = true;
        } else if (arg == "--repl-stop-on-any-error") {
            repl_stop_on_any_error = true;
        } else if (arg == "-t") {
            enable_trace = true;
        } else {
            fprintf(stderr, "Unknown option: %s\n", arg.c_str());
            exit(1);
        }
        a++;
    }

    struct ExecOptions options;
    options.enable_assert = enable_assert;
    options.enable_debug = enable_debug;
    options.enable_trace = enable_trace;

    if (a >= argc) {
        repl(argc, argv, options);
    }

    const std::string name = argv[a];
    auto i = name.find_last_of("/:\\");
    const std::string source_path { i != std::string::npos ? name.substr(0, i+1) : "" };

    CompilerSupport compiler_support(source_path, neonpath, nullptr, enable_debug);
    RuntimeSupport runtime_support(source_path, neonpath);
    std::unique_ptr<DebugInfo> debug;

    std::vector<unsigned char> bytecode;
    if (not has_suffix(name, ".neonx")) {

        std::stringstream source;
        if (name == "-") {
            source << std::cin.rdbuf();
        } else if (name == "-c") {
            source << argv[a+1];
        } else {
            std::ifstream inf(name);
            if (not inf) {
                fprintf(stderr, "Source file not found: %s\n", name.c_str());
                exit(1);
            }
            source << inf.rdbuf();
        }

        // TODO - Allow reading debug information from file.
        debug.reset(new DebugInfo(name, source.str()));

        try {
            auto tokens = tokenize(name, source.str());
            if (dump_tokens) {
                dump(*tokens);
            }

            auto parsetree = parse(*tokens);
            if (dump_parse) {
                dump(parsetree.get());
            }

            auto program = analyze(&compiler_support, parsetree.get());
            if (dump_ast) {
                dump(program);
            }

            bytecode = compile(program, debug.get());
            if (dump_listing) {
                disassemble(bytecode, std::cerr, debug.get());
            }

        } catch (CompilerError *error) {
            if (error_json) {
                error->write_json(std::cerr);
            } else {
                error->write(std::cerr);
            }
            exit(1);
        }

    } else {

        std::ifstream inf(name, std::ios::binary);
        if (not inf) {
            fprintf(stderr, "Source file not found: %s\n", name.c_str());
            exit(1);
        }
        std::stringstream object;
        object << inf.rdbuf();
        std::string s = object.str();
        std::copy(s.begin(), s.end(), std::back_inserter(bytecode));

    }

    exit(exec(name, bytecode, debug.get(), &runtime_support, &options, debug_port, argc-a, argv+a));
}
