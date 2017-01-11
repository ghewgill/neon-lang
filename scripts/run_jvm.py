import os
import sys

ExcludeTests = [
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
    "t/cal-test.neon",              # module os
    "t/cformat-test.neon",          # ModuleFunction
    "t/check.neon",                 # TryStatement
    "t/class-empty.neon",           # NewClassExpression
    "t/cmdline.neon",               # PredefinedVariable
    "t/comparison2.neon",           # ChainedComparisonExpression
    "t/complex-test.neon",          # ModuleFunction
    "t/compress-test.neon",         # compress.gzip
    "t/concat-bytes.neon",          # ConstantBytesExpression
    "t/conditional.neon",           # ConditionalExpression
    "t/datetime-test.neon",         # ModuleFunction
    "t/debug-example.neon",         # debugger.log
    "t/debug-server.neon",          # pointer
    "t/decimal.neon",               # exception
    "t/dictionary.neon",            # DictionaryReferenceIndexExpression
    "t/divide-by-zero.neon",        # TryStatement
    "t/encoding-base64.neon",       # ModuleFunction
    "t/enum.neon",                  # ConstantEnumExpression
    "t/equality.neon",              # ConstantBytesExpression
    "t/exception-as.neon",          # TryStatement
    "t/exception-code.neon",        # TryStatement
    "t/exception-function.neon",    # TryStatement
    "t/exception-nested.neon",      # TryStatement
    "t/exception-subexception.neon",# TryStatement
    "t/exception-trace.neon",       # RaiseStatement
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
    "t/file-writebytes.neon",       # file.writeBytes
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
    "t/forward.neon",               # segfault
    "t/function-pointer-nowhere.neon",  # ConstantNowhereExpression
    "t/function-pointer.neon",      # TypeFunctionPointer
    "t/gc-array.neon",              # NewClassExpression
    "t/gc-long-chain.neon",         # NewClassExpression
    "t/gc1.neon",                   # PredefinedVariable
    "t/gc2.neon",                   # PredefinedVariable
    "t/gc3.neon",                   # PredefinedVariable
    "t/hash-test.neon",             # ConstantBytesExpression
    "t/http-test.neon",             # pointer
    "t/import-dup.neon",            # module sys
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
    "t/lexer-unicode.neon",         # ConjunctionExpression
    "t/lexical-scope.neon",         # TryStatement
    "t/lisp-test.neon",             # ModuleFunction
    "t/literal-array.neon",         # ArrayComparisonExpression
    "t/literal.neon",               # DictionaryReferenceIndexExpression
    "t/logical.neon",               # ConjunctionExpression
    "t/logical2.neon",              # ConjunctionExpression
    "t/loop-label.neon",            # ArrayValueIndexExpression
    "t/math-test.neon",             # math.abs
    "t/methods-declare.neon",       # RecordLiteralExpression
    "t/methods.neon",               # RecordLiteralExpression
    "t/mkdir.neon",                 # TryStatement
    "t/mmap-test.neon",             # ConstantEnumExpression
    "t/module-alias.neon",          # module math
    "t/module-alias2.neon",         # ModuleVariable
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
    "t/new-init-module.neon",       # NewClassExpression
    "t/new-init.neon",              # NewClassExpression
    "t/number-ceil.neon",           # module math
    "t/number-underscore.neon",     # PredefinedVariable
    "t/os-test.neon",               # pointer
    "t/outer-parameter.neon",       # verifier
    "t/outer-tail.neon",            # verifier
    "t/outer.neon",                 # verifier
    "t/outer2.neon",                # verifier
    "t/parameter-out-array.neon",   # ArrayReferenceRangeExpression
    "t/parameter-out-string.neon",  # StringReferenceIndexExpression
    "t/parameters-ignore.neon",     # out parameters
    "t/parameters.neon",            # out parameters
    "t/pointer-method.neon",        # NewClassExpression
    "t/pointer-nil.neon",           # pointer
    "t/pointer-valid.neon",         # NewClassExpression
    "t/pointer.neon",               # NewClassExpression
    "t/pointer2.neon",              # NewClassExpression
    "t/pointer3.neon",              # pointer
    "t/pointer4.neon",              # NewClassExpression
    "t/pointer5.neon",              # pointer
    "t/pointer6.neon",              # pointer
    "t/pointer7.neon",              # pointer
    "t/pointer8.neon",              # NewClassExpression
    "t/predeclare1.neon",           # verifier
    "t/predeclare3.neon",           # dictionary__keys
    "t/process-test.neon",          # PredefinedVariable
    "t/record-empty.neon",          # RecordLiteralExpression
    "t/record-init.neon",           # RecordLiteralExpression
    "t/record-init2.neon",          # RecordLiteralExpression
    "t/record-private.neon",        # RecordLiteralExpression
    "t/recursion-limit.neon",       # segfault
    "t/recursion.neon",             # segfault
    "t/regex-test.neon",            # BooleanComparisonExpression
    "t/repl_enum_tostring.neon",    # ConstantEnumExpression
    "t/repl_import.neon",           # module random
    "t/repl_nested.neon",           # nested function
    "t/return-try.neon",            # TryStatement
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
    "t/stack-overflow.neon",        # segfault
    "t/string-bytes.neon",          # StringReferenceIndexExpression
    "t/string-escape.neon",         # StringReferenceIndexExpression
    "t/string-format.neon",         # format
    "t/string-multiline.neon",      # StringReferenceIndexExpression
    "t/string-test.neon",           # PredefinedVariable
    "t/strings.neon",               # StringReferenceIndexExpression
    "t/struct-test.neon",           # ModuleFunction
    "t/structure.neon",             # RecordLiteralExpression
    "t/sudoku-test.neon",           # ModuleFunction
    "t/sys-exit.neon",              # TryStatement
    "t/tail-call.neon",             # segfault
    "t/time-stopwatch.neon",        # RecordLiteralExpression
    "t/time-test.neon",             # module time
    "t/tostring.neon",              # ConstantEnumExpression
    "t/try-expression.neon",        # VariableExpression
    "t/type-nested.neon",           # RecordLiteralExpression
    "t/unicode-char.neon",          # StringReferenceIndexExpression
    "t/uninitialised-if-nested.neon",   # verifier
    "t/uninitialised-if.neon",      # verifier
    "t/uninitialised-out.neon",     # out parameter
    "t/uninitialised-repeat.neon",  # verifier
    "t/uninitialised-try.neon",     # TryStatement
    "t/valid-pointer.neon",         # NewClassExpression
    "t/value-index.neon",           # ArrayValueIndexExpression
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
