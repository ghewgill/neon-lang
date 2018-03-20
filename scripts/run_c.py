import os
import subprocess
import sys

ExcludeTests = [
    "t/array-fraction.neon",        # exception
    "t/array-negative.neon",        # exception
    "t/array-resize.neon",          # assert
    "t/array-subscript.neon",       # assert
    "t/array-tostring.neon",        # assert
    "t/array.neon",                 # exception
    "t/assert-enum.neon",           # assert
    "t/assert.neon",                # assert
    "t/assignment.neon",            # dictionary / classes
    "t/bigint-test.neon",           # module
    "t/binary-test.neon",           # module
    "t/cal-test.neon",              # os.system
    "t/case-overlap.neon",          # JUMPTBL
    "t/cformat-test.neon",          # module
    "t/check.neon",                 # exception
    "t/class-empty.neon",           # class
    "t/cmdline.neon",               # module
    "t/complex-test.neon",          # module
    "t/concat-bytes.neon",          # assert
    "t/datetime-test.neon",         # module
    "t/debug-example.neon",         # module
    "t/debug-server.neon",          # module
    "t/decimal.neon",               # arithmetic
    "t/dictionary-sorted.neon",     # assert
    "t/dictionary.neon",            # exception
    "t/divide-by-zero.neon",        # assert
    "t/encoding-base64.neon",       # assert
    "t/equality.neon",              # class
    "t/exception-as.neon",          # exception
    "t/exception-code.neon",        # exception
    "t/exception-function.neon",    # exception
    "t/exception-nested.neon",      # exception
    "t/exception-subexception.neon",# exception
    "t/exception-trace.neon",       # exception
    "t/exception.neon",             # exception
    "t/export-inline.neon",         # export
    "t/export-recursive.neon",      # export
    "t/export.neon",                # export
    "t/ffi.neon",                   # ffi
    "t/file-filecopied.neon",       # exception
    "t/file-filecopied1.neon",      # exception
    "t/file-filecopied2.neon",      # exception
    "t/file-filecopied3.neon",      # exception
    "t/file-symlink.neon",          # exception
    "t/file-test.neon",             # exception
    "t/file-writebytes.neon",       # file
    "t/file-writelines.neon",       # file
    "t/foreach-eval.neon",          # assert
    "t/fork.neon",                  # posix
    "t/function-default-out.neon",  # assert
    "t/function-pointer-nowhere.neon", # functionpointer
    "t/function-pointer.neon",      # functionpointer
    "t/gc-array.neon",              # class
    "t/gc-long-chain.neon",         # class
    "t/gc1.neon",                   # class
    "t/gc2.neon",                   # class
    "t/gc3.neon",                   # class
    "t/import.neon",                # exception
    "t/intdiv.neon",                # assert
    "t/interface-parameter-export.neon", # export
    "t/interface-parameter-export2.neon", # export
    "t/interface-parameter-import.neon", # class
    "t/interface-parameter-import2.neon", # class
    "t/interface.neon",             # class
    "t/intrinsic.neon",             # JUMPTBL
    "t/io-test.neon",               # module
    "t/json-test.neon",             # module
    "t/lexical-scope.neon",         # exception
    "t/literal-array.neon",         # assert
    "t/math-test.neon",             # precision
    "t/mkdir.neon",                 # exception
    "t/mmap-test.neon",             # mmap
    "t/module-alias2.neon",         # module
    "t/module-import-name-alias2.neon", # module
    "t/module-import-name2.neon",   # module
    "t/module.neon",                # module
    "t/module2.neon",               # exception
    "t/multiarray-test.neon",       # module
    "t/net-test.neon",              # net
    "t/new-init-module.neon",       # class
    "t/new-init.neon",              # class
    "t/number-ceil.neon",           # precision
    "t/number-underscore.neon",     # assert
    "t/os-test.neon",               # os
    "t/outer-parameter.neon",       # PUSHPOL
    "t/outer-tail.neon",            # PUSHPOL
    "t/outer.neon",                 # PUSHPOL
    "t/outer2.neon",                # PUSHPOL
    "t/parameters.neon",            # INOUT
    "t/pointer-method.neon",        # class
    "t/pointer-nil.neon",           # class
    "t/pointer-valid.neon",         # class
    "t/pointer.neon",               # class
    "t/pointer1.neon",              # class
    "t/pointer2.neon",              # class
    "t/pointer3.neon",              # class
    "t/pointer4.neon",              # class
    "t/pointer5.neon",              # class
    "t/pointer6.neon",              # class
    "t/pointer7.neon",              # class
    "t/pointer8.neon",              # class
    "t/process-test.neon",          # process
    "t/recursion-limit.neon",       # runtime
    "t/repl_import.neon",           # random
    "t/return-try.neon",            # exception
    "t/rtl.neon",                   # exception
    "t/sql-connect.neon",           # file
    "t/sql-cursor.neon",            # sqlite
    "t/sql-embed.neon",             # sqlite
    "t/sql-execute.neon",           # sqlite
    "t/sql-query.neon",             # sqlite
    "t/sql-whenever.neon",          # sqlite
    "t/sqlite-test.neon",           # sqlite
    "t/stack-overflow.neon",        # stack
    "t/string-bytes.neon",          # format
    "t/string-escape.neon",         # utf8
    "t/string-format.neon",         # format
    "t/string-slice.neon",          # assert
    "t/string-test.neon",           # assert
    "t/struct-test.neon",           # module
    "t/sys-exit.neon",              # exception
    "t/time-stopwatch.neon",        # module
    "t/try-expression.neon",        # exception
    "t/unicode-char.neon",          # utf8
    "t/uninitialised-case.neon",    # JUMPTBL
    "t/valid-pointer.neon",         # class
    "t/variant-test.neon",          # exception
    "t/while-valid.neon",           # class

    "lib/extsample/t/extsample-test.neon",        # module
]

fullname = sys.argv[1]
path, name = os.path.split(fullname)

if fullname.replace("\\", "/") in ExcludeTests:
    sys.exit(99)

subprocess.check_call([os.path.join("bin", "neonc"), "-q", fullname])
subprocess.check_call(["contrib/msvc/Debug/neonvm", fullname + "x"])
