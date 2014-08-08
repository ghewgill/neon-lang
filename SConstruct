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
    "interpreter.cpp",
    "lexer.cpp",
    "main.cpp",
    "parser.cpp",
    "util.cpp",
])
