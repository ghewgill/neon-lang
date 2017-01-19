import os
import sys

ExcludeTests = [
    "t/array-resize.neon",          # array default item
    "t/array-slice.neon",           # string append
    "t/array2d.neon",               # array copying semantics
    "t/assert-fail.neon",           # stderr
    "t/assert-fail2.neon",          # stderr
    "t/bigint-test.neon",           # ModuleFunction
    "t/cformat-test.neon",          # ModuleFunction
    "t/cmdline.neon",               # PredefinedVariable
    "t/complex-test.neon",          # ModuleFunction
    "t/compress-test.neon",         # compress.gzip
    "t/concat-bytes.neon",          # confusion between Bytes and String
    "t/datetime-test.neon",         # ModuleFunction
    "t/debug-example.neon",         # debugger.log
    "t/debug-server.neon",          # FunctionParameter
    "t/decimal.neon",               # exception
    "t/encoding-base64.neon",       # ModuleFunction
    "t/enum.neon",                  # enum toString
    "t/equality.neon",              # TypeFunctionPointer
    "t/exception-as.neon",          # record
    "t/exception-code.neon",        # record
    "t/export.neon",                # enum with no name
    "t/ffi.neon",                   # ExternalFunction
    "t/file.neon",                  # module file
    "t/file-exists.neon",           # FunctionParameter
    "t/file-filecopied.neon",       # FunctionParameter
    "t/file-filecopied1.neon",      # exception identification
    "t/file-filecopied2.neon",      # FunctionParameter
    "t/file-filecopied3.neon",      # FunctionParameter
    "t/file-symlink.neon",          # FunctionParameter
    "t/file-test.neon",             # FunctionParameter
    "t/file-writebytes.neon",       # FunctionParameter
    "t/file-writelines.neon",       # FunctionParameter
    "t/for.neon",                   # number format (2 vs 2.0)
    "t/fork.neon",                  # module posix
    "t/forth-test.neon",            # module os
    "t/forward.neon",               # segfault
    "t/function-pointer-nowhere.neon",  # ConstantNowhereExpression
    "t/function-pointer.neon",      # TypeFunctionPointer
    "t/gc-array.neon",              # module runtime
    "t/gc-long-chain.neon",         # module runtime
    "t/gc1.neon",                   # PredefinedVariable
    "t/gc2.neon",                   # PredefinedVariable
    "t/gc3.neon",                   # PredefinedVariable
    "t/hash-test.neon",             # sha-3
    "t/http-test.neon",             # module os
    "t/import-dup.neon",            # module sys
    "t/import.neon",                # ModuleVariable
    "t/indent.neon",                # verifier
    "t/index.neon",                 # record
    "t/inline-construct-record.neon",   # stack size too large
    "t/inline-init-record.neon",    # stack size too large
    "t/io-test.neon",               # PredefinedVariable
    "t/json-test.neon",             # ModuleFunction
    "t/lisp-test.neon",             # ModuleFunction
    "t/math-test.neon",             # math.abs
    "t/mkdir.neon",                 # TypeRecord
    "t/mmap-test.neon",             # module mmap
    "t/module-alias.neon",          # module math
    "t/module-alias2.neon",         # ModuleVariable
    "t/module-assign-let.neon",     # TypeRecord
    "t/module-assign-var.neon",     # TypeRecord
    "t/module-import-name-alias.neon",  # module math
    "t/module-import-name-alias2.neon", # ModuleVariable
    "t/module-import-name.neon",    # module math
    "t/module-import-name2.neon",   # ModuleVariable
    "t/module-scope.neon",          # TypeRecord
    "t/module.neon",                # TypeFunctionPointer
    "t/module2.neon",               # TypeFunctionPointer
    "t/modulo.neon",                # modulo
    "t/multiarray-test.neon",       # ModuleFunction
    "t/net-test.neon",              # record
    "t/new-init-module.neon",       # TypeFunctionPointer
    "t/number-ceil.neon",           # module math
    "t/number-underscore.neon",     # PredefinedVariable
    "t/os-test.neon",               # module os
    "t/outer-parameter.neon",       # verifier
    "t/outer-tail.neon",            # verifier
    "t/outer.neon",                 # verifier
    "t/outer2.neon",                # verifier
    "t/parameter-out-array.neon",   # out parameter
    "t/parameter-out-string.neon",  # out parameter
    "t/parameters-ignore.neon",     # out parameters
    "t/parameters.neon",            # out parameters
    "t/predeclare1.neon",           # verifier
    "t/process-test.neon",          # PredefinedVariable
    "t/recursion-limit.neon",       # segfault
    "t/recursion.neon",             # segfault
    "t/regex-test.neon",            # TypeRecord
    "t/repl_import.neon",           # module random
    "t/repl_nested.neon",           # nested function
    "t/rtl.neon",                   # TypeRecord
    "t/sodium-test.neon",           # TypeRecord
    "t/sql-connect.neon",           # ModuleVariable
    "t/sql-cursor.neon",            # ModuleVariable
    "t/sql-embed.neon",             # ModuleVariable
    "t/sql-execute.neon",           # ModuleVariable
    "t/sql-query.neon",             # ModuleVariable
    "t/sql-whenever.neon",          # ModuleVariable
    "t/sqlite-test.neon",           # bad type
    "t/stack-overflow.neon",        # segfault
    "t/string-bytes.neon",          # format
    "t/string-format.neon",         # format
    "t/string-test.neon",           # ModuleFunction
    "t/strings.neon",               # stack size too large
    "t/struct-test.neon",           # ModuleFunction
    "t/sudoku-test.neon",           # ModuleFunction
    "t/sys-exit.neon",              # TypeRecord
    "t/tail-call.neon",             # segfault
    "t/time-stopwatch.neon",        # imported record
    "t/time-test.neon",             # module time
    "t/tostring.neon",              # enum tostring
    "t/try-expression.neon",        # TryExpression
    "t/unicode-length.neon",        # actually works here so avoid the TODO
    "t/uninitialised-if-nested.neon",   # verifier
    "t/uninitialised-if.neon",      # verifier
    "t/uninitialised-out.neon",     # out parameter
    "t/uninitialised-repeat.neon",  # verifier
    "t/value-index.neon",           # TypeRecord
    "t/variant-test.neon",          # imported class name
    "t/win32-test.neon",            # ModuleFunction
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
