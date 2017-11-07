import os
import sys

ExcludeTests = [
    "t/array-fraction.neon",        # exception
    "t/array-negative.neon",        # exception
    "t/array-resize.neon",          # array default item
    "t/array-slice.neon",           # ArrayReferenceRangeExpression
    "t/array-tostring.neon",        # array comparison
    "t/array2d.neon",               # copy semantics
    "t/assert-enum.neon",           # enum
    "t/bigint-test.neon",           # module bigint
    "t/binary-test.neon",           # module binary
    "t/bytes-embed.neon",           # bytes
    "t/bytes-literal.neon",         # bytes
    "t/bytes-slice.neon",           # bytes
    "t/bytes-tostring.neon",        # bytes
    "t/bytes-value-index.neon",     # bytes
    "t/bytes.neon",                 # bytes
    "t/cal-test.neon",              # os.system
    "t/cformat-test.neon",          # module
    "t/cmdline.neon",               # command line format
    "t/complex-test.neon",          # module
    "t/compress-test.neon",         # module compress
    "t/concat-bytes.neon",          # bytes
    "t/datetime-test.neon",         # module
    "t/debug-example.neon",         # keyword in
    "t/debug-server.neon",          # StringReferenceIndexExpression
    "t/decimal.neon",               # Inf
    "t/dictionary.neon",            # exception on dictionary index not existing
    "t/encoding-base64.neon",       # module
    "t/enum.neon",                  # keyword enum
    "t/equality.neon",              # DictionaryComparisonExpression
    "t/exception-as.neon",          # exception offset
    "t/exception-subexception.neon",# exception multiple handlers
    "t/export.neon",                # method
    "t/extsample-test.neon",        # extension functions
    "t/ffi.neon",                   # ffi
    "t/file-exists.neon",           # module file
    "t/file-filecopied.neon",       # module file
    "t/file-filecopied1.neon",      # module file
    "t/file-filecopied2.neon",      # module file
    "t/file-filecopied3.neon",      # module file
    "t/file-symlink.neon",          # module file
    "t/file-test.neon",             # module file
    "t/file-writebytes.neon",       # module file
    "t/file-writelines.neon",       # module file
    "t/for.neon",                   # decimal floating point
    "t/foreach-string.neon",        # StringValueIndexExpression
    "t/forth-test.neon",            # os.system
    "t/function-default-out.neon",  # DummyExpression
    "t/function-pointer.neon",      # string.append
    "t/gc-array.neon",              # gc
    "t/gc-long-chain.neon",         # gc
    "t/gc1.neon",                   # gc
    "t/gc2.neon",                   # gc
    "t/gc3.neon",                   # gc
    "t/hash-test.neon",             # bytes
    "t/http-test.neon",             # StringReferenceIndexExpression
    "t/import.neon",                # import
    "t/interface-parameter-export.neon", # method
    "t/interface-parameter-import.neon", # method
    "t/interface-parameter-import2.neon", # method
    "t/in.neon",                    # ArrayInExpression
    "t/inc-reference.neon",         # actually works here so avoid the TODO
    "t/index.neon",                 # copy semantics
    "t/interface.neon",             # methods
    "t/intrinsic.neon",             # various
    "t/fork.neon",                  # module posix
    "t/io-test.neon",               # module io
    "t/json-test.neon",             # StringReferenceIndexExpression
    "t/lexer-unicode.neon",         # ArrayInExpression
    "t/lisp-test.neon",             # DictionaryInExpression
    "t/literal-array.neon",         # array comparison
    "t/loop-label.neon",            # break outer loop
    "t/math-test.neon",             # module math
    "t/methods-declare.neon",       # methods
    "t/methods.neon",               # methods
    "t/mkdir.neon",                 # module file
    "t/mmap-test.neon",             # module mmap
    "t/module-alias2.neon",         # keyword var
    "t/module-import-name-alias2.neon", # keyword var
    "t/module-import-name2.neon",   # keyword var
    "t/module.neon",                # methods
    "t/module2.neon",               # methods
    "t/modulo.neon",                # decimal floating point
    "t/multiarray-test.neon",       # module multiarray
    "t/net-test.neon",              # module net
    "t/number-ceil.neon",           # number formatting
    "t/os-test.neon",               # module os
    "t/parameter-out-array.neon",   # out parameters
    "t/parameter-out-string.neon",  # StringReferenceIndexExpression
    "t/parameters-ignore.neon",     # DummyExpression
    "t/parameters.neon",            # keyword in
    "t/pointer-method.neon",        # methods
    "t/process-test.neon",          # module process
    "t/record-private.neon",        # methods
    "t/recursion-limit.neon",       # module runtime
    "t/regex-test.neon",            # module regex
    "t/repl_import.neon",           # module random
    "t/retval-index.neon",          # StringValueIndexExpression
    "t/sodium-test.neon",           # module sodium
    "t/sql-connect.neon",           # module sqlite
    "t/sql-cursor.neon",            # module sqlite
    "t/sql-embed.neon",             # module sqlite
    "t/sql-execute.neon",           # module sqlite
    "t/sql-query.neon",             # module sqlite
    "t/sql-whenever.neon",          # module sqlite
    "t/sqlite-test.neon",           # module sqlite
    "t/stack-overflow.neon",        # catch stack overflow
    "t/string-bytes.neon",          # StringReferenceIndexExpression
    "t/string-escape.neon",         # StringReferenceIndexExpression
    "t/string-format.neon",         # format
    "t/string-multiline.neon",      # StringReferenceIndexExpression
    "t/string-slice.neon",          # StringReferenceIndexExpression
    "t/string-test.neon",           # module string
    "t/strings.neon",               # StringReferenceIndexExpression
    "t/struct-test.neon",           # DictionaryInExpression
    "t/sudoku-test.neon",           # ArrayReferenceRangeExpression
    "t/sys-exit.neon",              # module sys
    "t/tail-call.neon",             # recursion limit
    "t/time-stopwatch.neon",        # module time
    "t/time-test.neon",             # module time
    "t/tostring.neon",              # keyword enum
    "t/try-expression.neon",        # TryExpression
    "t/unicode-char.neon",          # StringReferenceIndexExpression
    "t/uninitialised-out.neon",     # out parameter
    "t/uninitialised-try.neon",     # js syntax error
    "t/value-index.neon",           # DictionaryValueIndexExpression
    "t/variant-test.neon",          # bytes
    "t/while-valid.neon",           # unknown
    "t/win32-test.neon",            # module win32
    "t/zeromq-test.neon",           # module zeromq
]

fullname = sys.argv[1]
path, name = os.path.split(fullname)

if fullname.replace("\\", "/") in ExcludeTests:
    sys.exit(99)

if os.name == "posix":
    node = "node"
elif os.path.exists("c:\\program files\\nodejs\\node.exe"):
    node = "\"c:\\program files\\nodejs\\node\""
else:
    sys.exit(99)

if os.system("{} -q -t js {}".format(os.path.join("bin", "neonc"), fullname)) != 0:
    sys.exit(1)
if os.system("{} {}".format(node, fullname.replace(".neon", ".js"))) != 0:
    sys.exit(1)
