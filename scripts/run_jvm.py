import os
import sys

ExcludeTests = [
    "t/array-fraction.neon",        # exception
    "t/array-negative.neon",        # exception
    "t/array-resize.neon",          # variable
    "t/array-slice.neon",           # ArrayReferenceRangeExpression
    "t/array-subscript.neon",       # variable
    "t/array-tostring.neon",        # ArrayComparisonExpression
    "t/array.neon",                 # TryStatement
    "t/array2d.neon",               # verifier: array element cast
    "t/assert-enum.neon",           # ConstantEnumExpression
    "t/assert-fail.neon",           # variable
    "t/assert-fail2.neon",          # variable
    "t/assert.neon",                # variable
    "t/assignment.neon",            # RecordLiteralExpression
    "t/bigint-test.neon",           # variable
    "t/binary-test.neon",           # module binary
    "t/bytes-embed.neon",           # ConstantBytesExpression
    "t/bytes-literal.neon",         # ConstantBytesExpression
    "t/bytes-slice.neon",           # ConstantBytesExpression
    "t/bytes-tostring.neon",        # ConstantBytesExpression
    "t/bytes.neon",                 # bytes
    "t/cal-test.neon",              # module os
    "t/cformat-test.neon",          # variable
    "t/check-if.neon",              # variable
    "t/check.neon",                 # variable
    "t/class-empty.neon",           # NewClassExpression
    "t/cmdline.neon",               # variable
    "t/comparison.neon",            # strb
    "t/comparison2.neon",           # ChainedComparisonExpression
    "t/complex-test.neon",          # variable
    "t/compress-test.neon",         # bytes
    "t/concat-bytes.neon",          # ConstantBytesExpression
    "t/conditional.neon",           # ConditionalExpression
    "t/const-boolean.neon",         # ConstantExpression
    "t/const-chain.neon",           # ConstantExpression
    "t/const-expression.neon",      # ConstantExpression
    "t/const-string.neon",          # ConstantExpression
    "t/const.neon",                 # ConstantExpression
    "t/datetime-test.neon",         # variable
    "t/debug-example.neon",         # variable
    "t/debug-server.neon",          # variable
    "t/decimal.neon",               # exception
    "t/dictionary.neon",            # DictionaryReferenceIndexExpression
    "t/divide-by-zero.neon",        # TryStatement
    "t/encoding-base64.neon",       # StringComparisonExpression
    "t/enum.neon",                  # ConstantEnumExpression
    "t/equality.neon",              # ConstantBytesExpression
    "t/exception-as.neon",          # TryStatement
    "t/exception-code.neon",        # TryStatement
    "t/exception-function.neon",    # TryStatement
    "t/exception-nested.neon",      # TryStatement
    "t/exception-subexception.neon",# TryStatement
    "t/exception-trace.neon",       # variable
    "t/exception.neon",             # TryStatement
    "t/export.neon",                # RecordLiteralExpression
    "t/ffi.neon",                   # variable
    "t/file.neon",                  # module file
    "t/file-exists.neon",           # module file
    "t/file-filecopied.neon",       # TryStatement
    "t/file-filecopied1.neon",      # TryStatement
    "t/file-filecopied2.neon",      # TryStatement
    "t/file-filecopied3.neon",      # TryStatement
    "t/file-symlink.neon",          # TryStatement
    "t/file-test.neon",             # TryStatement
    "t/file-writebytes.neon",       # bytes
    "t/file-writelines.neon",       # ArrayValueIndexExpression
    "t/for-scope.neon",             # duplicate field
    "t/for.neon",                   # duplicate field
    "t/foreach-eval.neon",          # ArrayValueIndexExpression
    "t/foreach-function.neon",      # ArrayValueIndexExpression
    "t/foreach-string.neon",        # StringValueIndexExpression
    "t/foreach-value.neon",         # ArrayValueIndexExpression
    "t/foreach.neon",               # ArrayValueIndexExpression
    "t/fork.neon",                  # module posix
    "t/forth-test.neon",            # module os
    "t/forward.neon",               # variable
    "t/function-args.neon",         # variable
    "t/function-defaultargs.neon",  # variable
    "t/function-local.neon",        # variable
    "t/function-namedargs.neon",    # variable
    "t/function-pointer-nowhere.neon",  # ConstantNowhereExpression
    "t/function-pointer.neon",      # variable
    "t/function.neon",              # variable
    "t/gc-array.neon",              # NewClassExpression
    "t/gc-long-chain.neon",         # NewClassExpression
    "t/gc1.neon",                   # variable
    "t/gc2.neon",                   # variable
    "t/gc3.neon",                   # variable
    "t/hash-test.neon",             # ConstantBytesExpression
    "t/http-test.neon",             # pointer
    "t/import-dup.neon",            # module sys
    "t/import-string.neon",         # ConstantExpression
    "t/import.neon",                # BooleanComparisonExpression
    "t/in.neon",                    # ArrayInExpression
    "t/indent.neon",                # verifier
    "t/index.neon",                 # RecordLiteralExpression
    "t/inline-construct-record.neon",   # RecordLiteralExpression
    "t/inline-init-record.neon",    # RecordLiteralExpression
    "t/intdiv.neon",                # variable
    "t/intrinsic.neon",             # chr
    "t/io-test.neon",               # variable
    "t/json-test.neon",             # variable
    "t/lexer-raw.neon",             # string__length
    "t/lexer-unicode.neon",         # ConstantExpression
    "t/lexical-scope.neon",         # TryStatement
    "t/lisp-test.neon",             # variable
    "t/literal-array.neon",         # ArrayComparisonExpression
    "t/literal.neon",               # DictionaryReferenceIndexExpression
    "t/local-clear.neon",           # variable
    "t/logical.neon",               # ConjunctionExpression
    "t/logical2.neon",              # ConjunctionExpression
    "t/loop-label.neon",            # ArrayValueIndexExpression
    "t/math-test.neon",             # ConstantExpression
    "t/methods-declare.neon",       # RecordLiteralExpression
    "t/methods.neon",               # RecordLiteralExpression
    "t/mkdir.neon",                 # TryStatement
    "t/mmap-test.neon",             # ConstantEnumExpression
    "t/module-alias.neon",          # module math
    "t/module-alias2.neon",         # ConstantExpression
    "t/module-import-name-alias.neon",  # module math
    "t/module-import-name-alias2.neon", # variable
    "t/module-import-name.neon",    # module math
    "t/module-import-name2.neon",   # variable
    "t/module.neon",                # ConstantEnumExpression
    "t/module2.neon",               # RecordLiteralExpression
    "t/modulo.neon",                # modulo
    "t/multiarray-test.neon",       # variable
    "t/nested-substitution.neon",   # DictionaryReferenceIndexExpression
    "t/net-test.neon",              # record
    "t/new-init-module.neon",       # variable
    "t/new-init.neon",              # variable
    "t/number-ceil.neon",           # module math
    "t/number-underscore.neon",     # variable
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
    "t/pointer-method.neon",        # NewClassExpression
    "t/pointer-nil.neon",           # ConstantNilExpression
    "t/pointer-valid.neon",         # NewClassExpression
    "t/pointer.neon",               # NewClassExpression
    "t/pointer2.neon",              # NewClassExpression
    "t/pointer3.neon",              # ConstantNilExpression
    "t/pointer4.neon",              # NewClassExpression
    "t/pointer5.neon",              # ConstantNilExpression
    "t/pointer6.neon",              # ConstantNilExpression
    "t/pointer7.neon",              # ConstantNilExpression
    "t/pointer8.neon",              # NewClassExpression
    "t/predeclare1.neon",           # verifier
    "t/predeclare2.neon",           # variable
    "t/predeclare3.neon",           # dictionary__keys
    "t/process-test.neon",          # bytes
    "t/record-empty.neon",          # RecordLiteralExpression
    "t/record-init.neon",           # RecordLiteralExpression
    "t/record-init2.neon",          # RecordLiteralExpression
    "t/record-private.neon",        # RecordLiteralExpression
    "t/recursion-limit.neon",       # variable
    "t/recursion.neon",             # variable
    "t/regex-test.neon",            # BooleanComparisonExpression
    "t/repl_enum_tostring.neon",    # ConstantEnumExpression
    "t/repl_function.neon",         # variable
    "t/repl_import.neon",           # module random
    "t/return-case.neon",           # variable
    "t/return-case2.neon",          # variable
    "t/return-if.neon",             # variable
    "t/return-loop.neon",           # variable
    "t/return-nothing.neon",        # variable
    "t/return-try.neon",            # variable
    "t/retval-index.neon",          # StringValueIndexExpression
    "t/rtl.neon",                   # TryStatement
    "t/shortcut.neon",              # ConjunctionExpression
    "t/sodium-test.neon",           # ConditionalExpression
    "t/sql-connect.neon",           # variable
    "t/sql-cursor.neon",            # variable
    "t/sql-embed.neon",             # variable
    "t/sql-execute.neon",           # variable
    "t/sql-query.neon",             # variable
    "t/sql-whenever.neon",          # variable
    "t/sqlite-test.neon",           # pointer
    "t/stack-overflow.neon",        # variable
    "t/string-bytes.neon",          # bytes
    "t/string-escape.neon",         # StringReferenceIndexExpression
    "t/string-format.neon",         # format
    "t/string-multiline.neon",      # StringReferenceIndexExpression
    "t/string-test.neon",           # variable
    "t/strings.neon",               # StringReferenceIndexExpression
    "t/struct-test.neon",           # variable
    "t/structure.neon",             # RecordLiteralExpression
    "t/sudoku-test.neon",           # variable
    "t/sys-exit.neon",              # TryStatement
    "t/tail-call.neon",             # variable
    "t/time-stopwatch.neon",        # RecordLiteralExpression
    "t/time-test.neon",             # module time
    "t/tostring.neon",              # ConstantEnumExpression
    "t/try-expression.neon",        # VariableExpression
    "t/type-compat.neon",           # variable
    "t/type-nested.neon",           # RecordLiteralExpression
    "t/unicode-char.neon",          # StringReferenceIndexExpression
    "t/uninitialised-function.neon",# variable
    "t/uninitialised-if-nested.neon",   # verifier
    "t/uninitialised-if.neon",      # verifier
    "t/uninitialised-out.neon",     # variable
    "t/uninitialised-repeat.neon",  # verifier
    "t/uninitialised-try.neon",     # TryStatement
    "t/unused-parameter.neon",      # variable
    "t/valid-pointer.neon",         # NewClassExpression
    "t/value-index.neon",           # ArrayValueIndexExpression
    "t/value-method.neon",          # variable
    "t/value-method2.neon",         # variable
    "t/variant-test.neon",          # RecordLiteralExpression
    "t/while-valid.neon",           # NewClassExpression
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
