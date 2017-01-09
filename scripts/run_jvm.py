import os
import sys

ExcludeTests = [
    "t/array-fraction.neon",        # exception
    "t/array-negative.neon",        # exception
    "t/array-resize.neon",          # PredefinedVariable
    "t/array-slice.neon",           # ArrayReferenceRangeExpression
    "t/array-subscript.neon",       # PredefinedVariable
    "t/array-tostring.neon",        # ArrayComparisonExpression
    "t/array.neon",                 # TryStatement
    "t/array2d.neon",               # verifier: array element cast
    "t/assert-enum.neon",           # ConstantEnumExpression
    "t/assert-fail.neon",           # PredefinedVariable
    "t/assert-fail2.neon",          # PredefinedVariable
    "t/assert.neon",                # PredefinedVariable
    "t/assignment.neon",            # RecordLiteralExpression
    "t/bigint-test.neon",           # ModuleFunction
    "t/binary-test.neon",           # module binary
    "t/bytes-embed.neon",           # ConstantBytesExpression
    "t/bytes-literal.neon",         # ConstantBytesExpression
    "t/bytes-slice.neon",           # ConstantBytesExpression
    "t/bytes-tostring.neon",        # ConstantBytesExpression
    "t/bytes.neon",                 # bytes
    "t/cal-test.neon",              # module os
    "t/cformat-test.neon",          # ModuleFunction
    "t/check-if.neon",              # Function
    "t/check.neon",                 # Function
    "t/class-empty.neon",           # NewClassExpression
    "t/cmdline.neon",               # PredefinedVariable
    "t/comparison.neon",            # strb
    "t/comparison2.neon",           # ChainedComparisonExpression
    "t/complex-test.neon",          # ModuleFunction
    "t/compress-test.neon",         # bytes
    "t/concat-bytes.neon",          # ConstantBytesExpression
    "t/conditional.neon",           # ConditionalExpression
    "t/const-boolean.neon",         # ConstantExpression
    "t/const-chain.neon",           # ConstantExpression
    "t/const-expression.neon",      # ConstantExpression
    "t/const-string.neon",          # ConstantExpression
    "t/const.neon",                 # ConstantExpression
    "t/datetime-test.neon",         # ModuleFunction
    "t/debug-example.neon",         # Function
    "t/debug-server.neon",          # ConstantExpression
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
    "t/exception-trace.neon",       # Function
    "t/exception.neon",             # TryStatement
    "t/export.neon",                # RecordLiteralExpression
    "t/ffi.neon",                   # ExternalFunction
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
    "t/forward.neon",               # Function
    "t/function-args.neon",         # Function
    "t/function-defaultargs.neon",  # Function
    "t/function-local.neon",        # Function
    "t/function-namedargs.neon",    # Function
    "t/function-pointer-nowhere.neon",  # ConstantNowhereExpression
    "t/function-pointer.neon",      # Function
    "t/function.neon",              # Function
    "t/gc-array.neon",              # NewClassExpression
    "t/gc-long-chain.neon",         # NewClassExpression
    "t/gc1.neon",                   # PredefinedVariable
    "t/gc2.neon",                   # PredefinedVariable
    "t/gc3.neon",                   # PredefinedVariable
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
    "t/intdiv.neon",                # PredefinedVariable
    "t/intrinsic.neon",             # chr
    "t/io-test.neon",               # PredefinedVariable
    "t/json-test.neon",             # ModuleFunction
    "t/lexer-raw.neon",             # string__length
    "t/lexer-unicode.neon",         # ConstantExpression
    "t/lexical-scope.neon",         # TryStatement
    "t/lisp-test.neon",             # Function
    "t/literal-array.neon",         # ArrayComparisonExpression
    "t/literal.neon",               # DictionaryReferenceIndexExpression
    "t/local-clear.neon",           # Function
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
    "t/module-import-name-alias2.neon", # ModuleVariable
    "t/module-import-name.neon",    # module math
    "t/module-import-name2.neon",   # ModuleVariable
    "t/module.neon",                # ConstantEnumExpression
    "t/module2.neon",               # RecordLiteralExpression
    "t/modulo.neon",                # modulo
    "t/multiarray-test.neon",       # ModuleFunction
    "t/nested-substitution.neon",   # DictionaryReferenceIndexExpression
    "t/net-test.neon",              # record
    "t/new-init-module.neon",       # Function
    "t/new-init.neon",              # Function
    "t/number-ceil.neon",           # module math
    "t/number-underscore.neon",     # PredefinedVariable
    "t/os-test.neon",               # pointer
    "t/outer-parameter.neon",       # Function
    "t/outer-tail.neon",            # Function
    "t/outer.neon",                 # Function
    "t/outer2.neon",                # Function
    "t/parameter-inout-array.neon", # Function
    "t/parameter-inout-string.neon",# Function
    "t/parameter-out-array.neon",   # Function
    "t/parameter-out-string.neon",  # Function
    "t/parameters-ignore.neon",     # Function
    "t/parameters.neon",            # Function
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
    "t/predeclare2.neon",           # Function
    "t/predeclare3.neon",           # dictionary__keys
    "t/process-test.neon",          # bytes
    "t/record-empty.neon",          # RecordLiteralExpression
    "t/record-init.neon",           # RecordLiteralExpression
    "t/record-init2.neon",          # RecordLiteralExpression
    "t/record-private.neon",        # RecordLiteralExpression
    "t/recursion-limit.neon",       # Function
    "t/recursion.neon",             # Function
    "t/regex-test.neon",            # BooleanComparisonExpression
    "t/repl_enum_tostring.neon",    # ConstantEnumExpression
    "t/repl_function.neon",         # Function
    "t/repl_import.neon",           # module random
    #"t/return-case.neon",           # variable
    "t/return-case2.neon",          # Function
    "t/return-if.neon",             # Function
    "t/return-loop.neon",           # Function
    "t/return-nothing.neon",        # Function
    "t/return-try.neon",            # Function
    "t/retval-index.neon",          # StringValueIndexExpression
    "t/rtl.neon",                   # TryStatement
    "t/shortcut.neon",              # ConjunctionExpression
    "t/sodium-test.neon",           # ConditionalExpression
    "t/sql-connect.neon",           # ModuleVariable
    "t/sql-cursor.neon",            # ModuleVariable
    "t/sql-embed.neon",             # ModuleVariable
    "t/sql-execute.neon",           # ModuleVariable
    "t/sql-query.neon",             # ModuleVariable
    "t/sql-whenever.neon",          # ModuleVariable
    "t/sqlite-test.neon",           # pointer
    "t/stack-overflow.neon",        # Function
    "t/string-bytes.neon",          # bytes
    "t/string-escape.neon",         # StringReferenceIndexExpression
    "t/string-format.neon",         # format
    "t/string-multiline.neon",      # StringReferenceIndexExpression
    "t/string-test.neon",           # PredefinedVariable
    "t/strings.neon",               # StringReferenceIndexExpression
    "t/struct-test.neon",           # ModuleFunction
    "t/structure.neon",             # RecordLiteralExpression
    "t/sudoku-test.neon",           # ModuleFunction
    "t/sys-exit.neon",              # TryStatement
    "t/tail-call.neon",             # Function
    "t/time-stopwatch.neon",        # RecordLiteralExpression
    "t/time-test.neon",             # module time
    "t/tostring.neon",              # ConstantEnumExpression
    "t/try-expression.neon",        # VariableExpression
    "t/type-compat.neon",           # Function
    "t/type-nested.neon",           # RecordLiteralExpression
    "t/unicode-char.neon",          # StringReferenceIndexExpression
    "t/uninitialised-function.neon",# Function
    "t/uninitialised-if-nested.neon",   # verifier
    "t/uninitialised-if.neon",      # verifier
    "t/uninitialised-out.neon",     # Function
    "t/uninitialised-repeat.neon",  # verifier
    "t/uninitialised-try.neon",     # TryStatement
    "t/unused-parameter.neon",      # Function
    "t/valid-pointer.neon",         # NewClassExpression
    "t/value-index.neon",           # ArrayValueIndexExpression
    "t/value-method.neon",          # Function
    "t/value-method2.neon",         # Function
    "t/variant-test.neon",          # RecordLiteralExpression
    "t/while-valid.neon",           # NewClassExpression
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
