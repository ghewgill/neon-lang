env = Environment()

env.Append(CXXFLAGS=[
    "-Wall",
    "-Wextra",
    "-Weffc++",
    "-Werror",
    "-pedantic",
    "-Wno-c++11-extensions",
    "-Wno-unused-parameter",
    "-g",
])

env.Program("simple", [
    "ast.cpp",
    "bytecode.cpp",
    "compiler.cpp",
    "disassembler.cpp",
    "exec.cpp",
    "interpreter.cpp",
    "lexer.cpp",
    "main.cpp",
    "parser.cpp",
    "util.cpp",
])

env.Program([
    "test_lexer.cpp",
    "lexer.cpp",
    "util.cpp",
])
env.Command("test_lexer.out", "test_lexer", "./test_lexer")

env.Program([
    "test_parser.cpp",
    "ast.cpp",
    "compiler.cpp",
    "interpreter.cpp",
    "parser.cpp",
    "lexer.cpp",
    "util.cpp",
])
env.Command("test_parser.out", "test_parser", "./test_parser")

env.Program([
    "test_interpreter.cpp",
    "ast.cpp",
    "compiler.cpp",
    "interpreter.cpp",
    "lexer.cpp",
    "parser.cpp",
    "util.cpp",
])
env.Command("test_interpreter.out", "test_interpreter", "./test_interpreter")

env.Program([
    "test_compiler.cpp",
    "ast.cpp",
    "bytecode.cpp",
    "compiler.cpp",
    "disassembler.cpp",
    "interpreter.cpp",
    "lexer.cpp",
    "parser.cpp",
    "util.cpp",
])
env.Command("test_compiler.out", "test_compiler", "./test_compiler")
