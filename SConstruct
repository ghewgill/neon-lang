import os
from SCons.Script.SConscript import SConsEnvironment

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
    "lexer.cpp",
    "main.cpp",
    "parser.cpp",
    "util.cpp",
])

def UnitTest(env, target, source, **kwargs):
    t = env.Program(target, source, **kwargs)
    # see the following for the reason why this lambda is necessary:
    # http://stackoverflow.com/questions/8219743/scons-addpostaction-causes-dependency-check-error-work-around
    env.AddPostAction(t, lambda *_, **__: os.system(t[0].abspath))
    env.Alias("test", t)
    return t

SConsEnvironment.UnitTest = UnitTest

env.UnitTest("test_lexer", [
    "test_lexer.cpp",
    "lexer.cpp",
    "util.cpp",
])

env.UnitTest("test_parser", [
    "test_parser.cpp",
    "ast.cpp",
    "compiler.cpp",
    "parser.cpp",
    "lexer.cpp",
    "util.cpp",
])

env.UnitTest("test_compiler", [
    "test_compiler.cpp",
    "ast.cpp",
    "bytecode.cpp",
    "compiler.cpp",
    "disassembler.cpp",
    "lexer.cpp",
    "parser.cpp",
    "util.cpp",
])
