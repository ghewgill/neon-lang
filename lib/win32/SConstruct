import sys

if sys.platform == "win32":
    env = Environment()
    env.Append(CXXFLAGS=["/EHsc", "/MDd", "/W4", "/WX"])

    env.Append(CPPPATH=["../../common"])
    env.SharedLibrary("neon_win32", ["win32.cpp"])
