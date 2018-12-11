import os
import subprocess
import sys

ExcludeTests = [
    "t/array2d.neon",               # copy semantics
    "t/array-fraction.neon",        # exception
    "t/array-negative.neon",        # exception
    "t/array.neon",                 # exception
    "t/array-resize.neon",          # import
    "t/array-subscript.neon",       # import
    "t/array-tostring.neon",        # import
    "t/assert-enum.neon",           # import
    "t/assert.neon",                # import
    "t/bigint.neon",                # import
    "t/binary-test.neon",           # binary
    "t/bytes-embed.neon",           # bytes
    "t/bytes-literal.neon",         # bytes
    "t/bytes.neon",                 # bytes
    "t/bytes-slice.neon",           # bytes
    "t/bytes-tostring.neon",        # bytes
    "t/bytes-value-index.neon",     # bytes
    "t/cal-test.neon",              # import
    "t/case-jumptbl.neon",          # jumptbl
    "t/case-overlap.neon",          # jumptbl
    "t/cformat-test.neon",          # import
    "t/check.neon",                 # exception
    "t/class-empty.neon",           # class
    "t/cmdline.neon",               # pushppg
    "t/complex-test.neon",          # import
    "t/concat-bytes.neon",          # import
    "t/datetime-test.neon",         # import
    "t/debug-example.neon",         # debugger
    "t/debug-server.neon",          # import
    "t/decimal.neon",               # decimal
    "t/dictionary.neon",            # exception
    "t/dictionary-sorted.neon",     # import
    "t/divide-by-zero.neon",        # exception
    "t/encoding-base64.neon",       # import
    "t/equality.neon",              # class
    "t/exception-as.neon",          # exception
    "t/exception-code.neon",        # exception
    "t/exception-function.neon",    # exception
    "t/exception.neon",             # exception
    "t/exception-nested.neon",      # exception
    "t/exception-opstack.neon",     # exception
    "t/exception-stackerror.neon",  # exception
    "t/exception-subexception.neon",# exception
    "t/exception-trace.neon",       # exception
    "t/export-function-indent.neon",# export function
    "t/export-inline.neon",         # export type
    "t/export.neon",                # export type
    "t/ffi.neon",                   # exception
    "t/file-exists.neon",           # import
    "t/file-filecopied1.neon",      # import
    "t/file-filecopied2.neon",      # import
    "t/file-filecopied3.neon",      # import
    "t/file-filecopied.neon",       # import
    "t/file-symlink.neon",          # import
    "t/file-test.neon",             # import
    "t/file-writebytes.neon",       # import
    "t/file-writelines.neon",       # import
    "t/foreach-eval.neon",          # import
    "t/fork.neon",                  # posix
    "t/for.neon",                   # decimal
    "t/function-pointer.neon",      # module
    "t/function-pointer-nowhere.neon", # module
    "t/gc1.neon",                   # import
    "t/gc2.neon",                   # import
    "t/gc3.neon",                   # import
    "t/gc-array.neon",              # import
    "t/gc-long-chain.neon",         # import
    "t/import-dup.neon",            # sys
    "t/import.neon",                # import
    "t/import-string.neon",         # import
    "t/index.neon",                 # copy semantics
    "t/in.neon",                    # map element address
    "t/intdiv.neon",                # import
    "t/interface.neon",             # class
    "t/interface-parameter-export2.neon",   # export function
    "t/interface-parameter-export.neon",    # typesize
    "t/interface-parameter-import2.neon",   # typesize
    "t/interface-parameter-import.neon",    # importsize
    "t/intrinsic.neon",             # jumptbl
    "t/io-test.neon",               # import
    "t/json-test.neon",             # import
    "t/lexical-scope.neon",         # exception
    "t/literal-array.neon",         # import
    "t/literal.neon",               # map element address
    "t/math-test.neon",             # math
    "t/mkdir.neon",                 # import
    "t/mmap-test.neon",             # import
    "t/module2.neon",               # import
    "t/module-alias2.neon",         # import
    "t/module-import-name2.neon",   # import
    "t/module-import-name-alias2.neon", # import
    "t/module.neon",                # typesize
    "t/modulo.neon",                # modulo
    "t/multiarray-test.neon",       # import
    "t/nested-substitution.neon",   # import
    "t/net-test.neon",              # import
    "t/net-test-udp.neon",          # import
    "t/new-init-module.neon",       # import
    "t/new-init.neon",              # import
    "t/number-ceil.neon",           # number formatting
    "t/number-underscore.neon",     # import
    "t/object-isa-case.neon",       # exception
    "t/object-isa.neon",            # import
    "t/object.neon",                # object
    "t/object-null.neon",           # object
    "t/object-operator.neon",       # object
    "t/os-test.neon",               # object
    "t/outer2.neon",                # pushpol
    "t/outer-issue192.neon",        # pushpol
    "t/outer.neon",                 # pushpol
    "t/outer-parameter.neon",       # pushpol
    "t/outer-tail.neon",            # pushpol
    "t/pointer2.neon",              # class
    "t/pointer3.neon",              # class
    "t/pointer4.neon",              # class
    "t/pointer5.neon",              # class
    "t/pointer6.neon",              # class
    "t/pointer7.neon",              # class
    "t/pointer8.neon",              # class
    "t/pointer-method.neon",        # class
    "t/pointer-mismatch.neon",      # class
    "t/pointer.neon",               # class
    "t/pointer-nil.neon",           # class
    "t/pointer-valid.neon",         # class
    "t/process-test.neon",          # import
    "t/recursion-limit.neon",       # runtime
    "t/repl_import.neon",           # import
    "t/return-try.neon",            # exception
    "t/rtl.neon",                   # exception
    "t/sql-connect.neon",           # import
    "t/sql-cursor.neon",            # import
    "t/sql-embed.neon",             # import
    "t/sql-execute.neon",           # import
    "t/sqlite-test.neon",           # sqlite
    "t/sql-query.neon",             # import
    "t/sql-whenever.neon",          # import
    "t/stack-overflow.neon",        # overflow
    "t/string-bytes.neon",          # bytes
    "t/string-escape.neon",         # utf8
    "t/string-format.neon",         # format
    "t/string-slice.neon",          # import
    "t/strings.neon",               # string splice
    "t/string-test.neon",           # import
    "t/struct-test.neon",           # import
    "t/sys-exit.neon",              # exception
    "t/time-stopwatch.neon",        # import
    "t/time-test.neon",             # import
    "t/try-expression.neon",        # import
    "t/unicode-char.neon",          # utf8
    "t/unicode-length.neon",        # import
    "t/uninitialised-case.neon",    # jumptbl
    "t/valid-pointer.neon",         # class
    "t/value-copy.neon",            # import
    "t/value-index.neon",           # dict
    "t/while-valid.neon",           # class

    "lib/compress/t/compress-test.neon",            # module
    "lib/extsample/t/extsample-test.neon",          # module
    "lib/hash/t/hash-test.neon",                    # module
    "lib/http/t/http-test.neon",                    # module
    "lib/regex/t/regex-test.neon",                  # module
    "lib/sodium/t/sodium-test.neon",                # module
    "lib/zeromq/t/zeromq-test.neon",                # module
]

fullname = sys.argv[1]
path, name = os.path.split(fullname)

if fullname.replace("\\", "/") in ExcludeTests:
    sys.exit(99)

subprocess.check_call([os.path.join("bin", "neonc"), "-q", fullname])
subprocess.check_call(["exec/gonex/gonex", fullname + "x"])
