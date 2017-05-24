import os
import sys

ExcludeTests = [
    "t/array-resize.neon",          # array default item
    "t/array2d.neon",               # array copying semantics
    "t/bigint-test.neon",           # illegal character dot
    "t/cformat-test.neon",          # module variant
    "t/cmdline.neon",               # PredefinedVariable sys$args
    "t/complex-test.neon",          # illegal character dot
    "t/compress-test.neon",         # compress.gzip
    "t/concat-bytes.neon",          # confusion between Bytes and String
    "t/datetime-test.neon",         # illegal character dot
    "t/debug-example.neon",         # debugger.log
    "t/debug-server.neon",          # illegal character dot
    "t/decimal.neon",               # exception
    "t/encoding-base64.neon",       # verifier - incompatible object for function call
    "t/enum.neon",                  # enum toString
    "t/exception-as.neon",          # ExceptionType
    "t/exception-code.neon",        # ExceptionType
    "t/export.neon",                # enum with no name
    "t/ffi.neon",                   # ForeignFunction
    "t/file-filecopied1.neon",      # exception identification
    "t/file-symlink.neon",          # file.symlink
    "t/file-test.neon",             # stack size
    "t/for.neon",                   # number format (2 vs 2.0)
    "t/fork.neon",                  # module posix
    "t/forth-test.neon",            # module os
    "t/function-default-out.neon",  # DummyExpression
    "t/gc-array.neon",              # module runtime
    "t/gc-long-chain.neon",         # module runtime
    "t/gc1.neon",                   # PredefinedVariable
    "t/gc2.neon",                   # PredefinedVariable
    "t/gc3.neon",                   # PredefinedVariable
    "t/hash-test.neon",             # sha-3
    "t/http-test.neon",             # module os
    "t/import.neon",                # illegal character dot
    "t/index.neon",                 # copy semantics
    "t/inline-construct-record.neon",   # stack size too large
    "t/inline-init-record.neon",    # stack size too large
    "t/io-test.neon",               # PredefinedVariable io$stdout
    "t/json-test.neon",             # illegal character dot
    "t/lisp-test.neon",             # store constant string expression?
    "t/math-test.neon",             # math.abs
    "t/mkdir.neon",                 # file.mkdir
    "t/mmap-test.neon",             # module mmap
    "t/module-alias.neon",          # module math
    "t/module-alias2.neon",         # module initialisation
    "t/module-import-name-alias.neon",  # module math
    "t/module-import-name-alias2.neon", # module initialisation
    "t/module-import-name.neon",    # module math
    "t/module-import-name2.neon",   # module initialisation
    "t/module.neon",                # NoClassDefFoundError
    "t/module2.neon",               # illegal character dot
    "t/modulo.neon",                # modulo
    "t/multiarray-test.neon",       # verifier - bad type in putfield
    "t/net-test.neon",              # illegal character dot
    "t/new-init-module.neon",       # NoClassDefFoundError
    "t/number-ceil.neon",           # module math
    "t/os-test.neon",               # module os
    "t/outer-parameter.neon",       # verifier
    "t/outer-tail.neon",            # verifier
    "t/outer.neon",                 # verifier
    "t/outer2.neon",                # verifier
    "t/parameter-inout-array.neon", # actually works here so avoid the TODO
    "t/parameter-inout-string.neon",# actually works here so avoid the TODO
    "t/process-test.neon",          # module process
    "t/recursion-limit.neon",       # module runtime
    "t/regex-test.neon",            # module regex
    "t/repl_import.neon",           # module random
    "t/repl_nested.neon",           # nested function
    "t/rtl.neon",                   # ExceptionType
    "t/sodium-test.neon",           # module sodium
    "t/sql-connect.neon",           # module sqlite
    "t/sql-cursor.neon",            # module sqlite
    "t/sql-embed.neon",             # module sqlite
    "t/sql-execute.neon",           # module sqlite
    "t/sql-query.neon",             # module sqlite
    "t/sql-whenever.neon",          # module sqlite
    "t/sqlite-test.neon",           # bad type
    "t/stack-overflow.neon",        # catch stack overflow
    "t/string-bytes.neon",          # format
    "t/string-format.neon",         # format
    "t/string-test.neon",           # module string
    "t/strings.neon",               # stack size too large
    "t/struct-test.neon",           # illegal character dot
    "t/sudoku-test.neon",           # FunctionParameter
    "t/sys-exit.neon",              # ExceptionType
    "t/tail-call.neon",             # stack overflow
    "t/time-stopwatch.neon",        # illegal character dot
    "t/time-test.neon",             # module time
    "t/tostring.neon",              # enum tostring
    "t/unicode-length.neon",        # actually works here so avoid the TODO
    "t/variant-test.neon",          # illegal character dot
    "t/win32-test.neon",            # illegal character dot
]

fullname = sys.argv[1]
path, name = os.path.split(fullname)

if fullname.replace("\\", "/") in ExcludeTests:
    sys.exit(99)

java = "java"
if os.name == "nt" and os.path.exists(r"c:\ProgramData\Oracle\Java\javapath\java.exe"):
    java = r"c:\ProgramData\Oracle\Java\javapath\java.exe"

if os.system("{} -q -t jvm {}".format(os.path.join("bin", "neonc"), fullname)) != 0:
    sys.exit(1)
classpath = path if path else "."
if os.system("{} -cp {} {}".format(java, os.pathsep.join([classpath, "jvm"]), name.replace(".neon", ""))) != 0:
    sys.exit(1)
