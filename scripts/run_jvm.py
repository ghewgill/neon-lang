import os
import sys

ExcludeTests = [
    "t/array-fraction.neon",        # exception
    "t/array-negative.neon",        # exception
    "t/array-resize.neon",          # expression
    "t/array-slice.neon",           # expression
    "t/array-subscript.neon",       # variable
    "t/array-tostring.neon",        # expression
    "t/array.neon",                 # TryStatement
    "t/array2d.neon",               # verifier: array element cast
    "t/assert-enum.neon",           # expression
    "t/assert-fail.neon",           # variable
    "t/assert-fail2.neon",          # variable
    "t/assert.neon",                # variable
    "t/assignment.neon",            # boolean
    "t/bigint-test.neon",           # variable
    "t/binary-test.neon",           # module binary
    "t/boolean.neon",               # boolean
    "t/bytes-embed.neon",           # expression
    "t/bytes-literal.neon",         # expression
    "t/bytes-slice.neon",           # expression
    "t/bytes-tostring.neon",        # expression
    "t/bytes.neon",                 # bytes
    "t/cal-test.neon",              # module os
    #"t/case3.neon",                 # statement
    "t/cformat-test.neon",          # variable
    "t/check-if.neon",              # variable
    "t/check.neon",                 # variable
    "t/class-empty.neon",           # expression
    "t/cmdline.neon",               # variable
    "t/comparison.neon",            # strb
    "t/comparison2.neon",           # expression
    "t/complex-test.neon",          # variable
    "t/compress-test.neon",         # bytes
    "t/concat-bytes.neon",          # expression
    "t/conditional.neon",           # expression
    "t/const-boolean.neon",         # expression
    "t/const-chain.neon",           # expression
    "t/const-expression.neon",      # expression
    "t/const-string.neon",          # statement
    "t/const.neon",                 # expression
    "t/datetime-test.neon",         # variable
    "t/debug-example.neon",         # variable
    "t/debug-server.neon",          # variable
    "t/decimal.neon",               # exception
    "t/dictionary-keys.neon",       # expression
    "t/dictionary.neon",            # expression
    "t/divide-by-zero.neon",        # statement
    "t/encoding-base64.neon",       # statement
    "t/enum.neon",                  # expression
    "t/equality.neon",              # boolean
    "t/exception-as.neon",          # statement
    "t/exception-code.neon",        # statement
    "t/exception-function.neon",    # statement
    "t/exception-nested.neon",      # statement
    "t/exception-subexception.neon",# statement
    "t/exception-trace.neon",       # variable
    "t/exception.neon",             # statement
    "t/export.neon",                # expression
    "t/ffi.neon",                   # variable
    "t/file.neon",                  # module file
    "t/file-exists.neon",           # module file
    "t/file-filecopied.neon",       # statement
    "t/file-filecopied1.neon",      # statement
    "t/file-filecopied2.neon",      # statement
    "t/file-filecopied3.neon",      # statement
    "t/file-symlink.neon",          # statement
    "t/file-test.neon",             # statement
    "t/file-writebytes.neon",       # bytes
    "t/file-writelines.neon",       # expression
    "t/for-bounds.neon",            # statement
    "t/for-scope.neon",             # duplicate field
    "t/for.neon",                   # duplicate field
    "t/foreach-eval.neon",          # expression
    "t/foreach-function.neon",      # expression
    "t/foreach-string.neon",        # expression
    "t/foreach-value.neon",         # expression
    "t/foreach.neon",               # expression
    "t/fork.neon",                  # module posix
    "t/forth-test.neon",            # module os
    "t/forward.neon",               # variable
    "t/function-args.neon",         # variable
    "t/function-defaultargs.neon",  # variable
    "t/function-local.neon",        # variable
    "t/function-namedargs.neon",    # variable
    "t/function-pointer-nowhere.neon",  # expression
    "t/function-pointer.neon",      # variable
    "t/function.neon",              # variable
    "t/gc-array.neon",              # expression
    "t/gc-long-chain.neon",         # expression
    "t/gc1.neon",                   # statement
    "t/gc2.neon",                   # statement
    "t/gc3.neon",                   # statement
    "t/hash-test.neon",             # expression
    "t/http-test.neon",             # pointer
    "t/import-dup.neon",            # module sys
    "t/import-string.neon",         # expression
    "t/import.neon",                # statement
    "t/in.neon",                    # expression
    "t/inc-reference.neon",         # statement
    "t/inc.neon",                   # statement
    "t/indent.neon",                # verifier
    "t/index.neon",                 # expression
    "t/inline-construct-record.neon",   # expression
    "t/inline-init-record.neon",    # expression
    "t/intdiv.neon",                # statement
    "t/intrinsic.neon",             # statement
    "t/io-test.neon",               # variable
    "t/json-test.neon",             # variable
    "t/lexer-raw.neon",             # string__length
    "t/lexer-unicode.neon",         # expression
    "t/lexical-scope.neon",         # statement
    "t/lisp-test.neon",             # variable
    "t/literal-array.neon",         # statement
    "t/literal-empty.neon",         # expression
    "t/literal.neon",               # expression
    "t/local-clear.neon",           # variable
    "t/logical.neon",               # expression
    "t/logical2.neon",              # expression
    "t/loop-label.neon",            # statement
    "t/math-test.neon",             # expression
    "t/methods-declare.neon",       # expression
    "t/methods.neon",               # expression
    "t/mkdir.neon",                 # statement
    "t/mmap-test.neon",             # expression
    "t/module-alias.neon",          # module math
    "t/module-alias2.neon",         # expression
    "t/module-import-name-alias.neon",  # module math
    "t/module-import-name-alias2.neon", # variable
    "t/module-import-name.neon",    # module math
    "t/module-import-name2.neon",   # variable
    "t/module.neon",                # expression
    "t/module2.neon",               # expression
    "t/modulo.neon",                # modulo
    "t/multiarray-test.neon",       # variable
    "t/nested-substitution.neon",   # expression
    "t/net-test.neon",              # record
    "t/new-init-module.neon",       # variable
    "t/new-init.neon",              # variable
    "t/next-for.neon",              # statement
    "t/next-while.neon",            # statement
    "t/number-ceil.neon",           # module math
    "t/number-underscore.neon",     # statement
    "t/os-test.neon",               # pointer
    "t/outer-parameter.neon",       # variable
    "t/outer-tail.neon",            # variable
    "t/outer.neon",                 # variable
    "t/outer2.neon",                # variable
    "t/parameter-inout-array.neon", # variable
    "t/parameter-inout-string.neon",# variable
    "t/parameter-out-array.neon",   # variable
    "t/parameter-out-string.neon",  # variable
    "t/parameters-ignore.neon",     # variable
    "t/parameters.neon",            # variable
    "t/pointer-method.neon",        # expression
    "t/pointer-nil.neon",           # expression
    "t/pointer-valid.neon",         # expression
    "t/pointer.neon",               # expression
    "t/pointer2.neon",              # expression
    "t/pointer3.neon",              # expression
    "t/pointer4.neon",              # expression
    "t/pointer5.neon",              # expression
    "t/pointer6.neon",              # expression
    "t/pointer7.neon",              # expression
    "t/pointer8.neon",              # expression
    "t/predeclare1.neon",           # verifier
    "t/predeclare2.neon",           # variable
    "t/predeclare3.neon",           # expression
    "t/process-test.neon",          # bytes
    "t/record-empty.neon",          # expression
    "t/record-init.neon",           # expression
    "t/record-init2.neon",          # expression
    "t/record-private.neon",        # expression
    "t/recursion-limit.neon",       # variable
    "t/recursion.neon",             # variable
    "t/regex-test.neon",            # statement
    "t/repeat-next.neon",           # statement
    "t/repl_enum_tostring.neon",    # expression
    "t/repl_function.neon",         # variable
    "t/repl_import.neon",           # module random
    "t/return-case.neon",           # variable
    "t/return-case2.neon",          # variable
    "t/return-if.neon",             # variable
    "t/return-loop.neon",           # variable
    "t/return-nothing.neon",        # variable
    "t/return-try.neon",            # variable
    "t/retval-index.neon",          # expression
    "t/rtl.neon",                   # statement
    "t/shortcut.neon",              # expression
    "t/sodium-test.neon",           # boolean
    "t/sql-connect.neon",           # variable
    "t/sql-cursor.neon",            # variable
    "t/sql-embed.neon",             # variable
    "t/sql-execute.neon",           # variable
    "t/sql-query.neon",             # variable
    "t/sql-whenever.neon",          # variable
    "t/sqlite-test.neon",           # pointer
    "t/stack-overflow.neon",        # variable
    "t/string-bytes.neon",          # bytes
    "t/string-escape.neon",         # expression
    "t/string-format.neon",         # format
    "t/string-multiline.neon",      # expression
    "t/string-test.neon",           # statement
    "t/strings.neon",               # expression
    "t/struct-test.neon",           # variable
    "t/structure.neon",             # expression
    "t/sudoku-test.neon",           # variable
    "t/sys-exit.neon",              # statement
    "t/tail-call.neon",             # variable
    "t/time-stopwatch.neon",        # expression
    "t/time-test.neon",             # module time
    "t/tostring.neon",              # boolean
    "t/try-expression.neon",        # variable
    "t/type-compat.neon",           # variable
    "t/type-nested.neon",           # expression
    "t/unicode-char.neon",          # expression
    "t/unicode-length.neon",        # statement
    "t/uninitialised-case.neon",    # statement
    "t/uninitialised-function.neon",# variable
    "t/uninitialised-if-nested.neon",   # verifier
    "t/uninitialised-if.neon",      # verifier
    "t/uninitialised-out.neon",     # variable
    "t/uninitialised-repeat.neon",  # verifier
    "t/uninitialised-try.neon",     # statement
    "t/unused-parameter.neon",      # variable
    "t/valid-pointer.neon",         # expression
    "t/value-index.neon",           # expression
    "t/value-method.neon",          # variable
    "t/value-method2.neon",         # variable
    "t/variant-test.neon",          # expression
    "t/while-valid.neon",           # expression
    "t/win32-test.neon",            # variable
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
