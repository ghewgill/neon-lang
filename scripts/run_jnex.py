import os
import subprocess
import sys

ExcludeTests = [
    "t/arithmetic.neon",            # BigDecimalMath.pow
    "t/arithmetic2.neon",           # BigDecimalMath.pow
    "t/array-resize.neon",          # import
    "t/array-subscript.neon",       # import
    "t/array-tostring.neon",        # import
    "t/array2d.neon",               # copy semantics
    "t/assert-enum.neon",           # import
    "t/assert.neon",                # import
    "t/bigint-test.neon",           # import
    "t/binary-test.neon",           # binary
    "t/bytes-literal.neon",         # array
    "t/cal-test.neon",              # import
    "t/cformat-test.neon",          # import
    "t/cmdline.neon",               # PUSHPPG
    "t/complex-test.neon",          # import
    "t/concat-bytes.neon",          # import
    "t/datetime-test.neon",         # import
    "t/debug-example.neon",         # debug module
    "t/debug-server.neon",          # import
    "t/decimal.neon",               # number format
    "t/dictionary-sorted.neon",     # import
    "t/encoding-base64.neon",       # import
    "t/equality.neon",              # PUSHM
    "t/export-function-indent.neon",# function_export
    "t/export-inline.neon",         # types
    "t/export.neon",                # types
    "t/ffi.neon",                   # CALLE
    "t/file-exists.neon",           # file
    "t/file-filecopied.neon",       # file
    "t/file-filecopied1.neon",      # file
    "t/file-filecopied2.neon",      # file
    "t/file-filecopied3.neon",      # file
    "t/file-symlink.neon",          # file
    "t/file-test.neon",             # file
    "t/file-writebytes.neon",       # file
    "t/file-writelines.neon",       # file
    "t/foreach-eval.neon",          # import
    "t/fork.neon",                  # posix
    "t/function-pointer-nowhere.neon", # EQA
    "t/function-pointer.neon",      # PUSHM
    "t/gc-array.neon",              # import
    "t/gc-long-chain.neon",         # import
    "t/gc1.neon",                   # import
    "t/gc2.neon",                   # import
    "t/gc3.neon",                   # import
    "t/import-dup.neon",            # sys
    "t/import-string.neon",         # import
    "t/import.neon",                # import
    "t/in.neon",                    # INA
    "t/index.neon",                 # copy semantics
    "t/intdiv.neon",                # import
    "t/interface-parameter-export.neon",    # types
    "t/interface-parameter-export2.neon",   # function_export
    "t/interface-parameter-import.neon",    # import
    "t/interface-parameter-import2.neon",   # types
    "t/interface.neon",             # SWAP
    "t/intrinsic.neon",             # format
    "t/io-test.neon",               # import
    "t/json-test.neon",             # import
    "t/lexer-unicode.neon",         # INA
    "t/literal-array.neon",         # import
    "t/math-test.neon",             # math
    "t/mkdir.neon",                 # file
    "t/mmap-test.neon",             # import
    "t/module-alias.neon",          # math
    "t/module-alias2.neon",         # module
    "t/module-assign-let.neon",     # module
    "t/module-assign-var.neon",     # module
    "t/module-import-name-alias.neon",  # module
    "t/module-import-name-alias2.neon", # module
    "t/module-import-name.neon",    # module
    "t/module-import-name2.neon",   # module
    "t/module-import-name3.neon",   # module
    "t/module-scope.neon",          # module
    "t/module.neon",                # module
    "t/module2.neon",               # module
    "t/modulo.neon",                # modulo
    "t/multiarray-test.neon",       # import
    "t/net-test.neon",              # import
    "t/new-init-module.neon",       # module
    "t/number-ceil.neon",           # math
    "t/os-test.neon",               # os
    "t/outer-issue192.neon",        # PUSHPOL
    "t/outer-parameter.neon",       # PUSHPOL
    "t/outer-tail.neon",            # PUSHPOL
    "t/outer.neon",                 # PUSHPOL
    "t/outer2.neon",                # PUSHPOL
    "t/parameter-out-array.neon",   # array__splice
    "t/parameter-out-string.neon",  # string__splice
    "t/process-test.neon",          # process
    "t/recursion-limit.neon",       # runtime
    "t/repl_import.neon",           # random
    "t/rtl.neon",                   # max
    "t/sql-connect.neon",           # file
    "t/sql-cursor.neon",            # sqlite
    "t/sql-embed.neon",             # sqlite
    "t/sql-execute.neon",           # sqlite
    "t/sql-query.neon",             # sqlite
    "t/sql-whenever.neon",          # sqlite
    "t/sqlite-test.neon",           # sqlite
    "t/stack-overflow.neon",        # stack overflow
    "t/string-bytes.neon",          # format
    "t/string-escape.neon",         # UTF-8
    "t/string-format.neon",         # format
    "t/string-slice.neon",          # string__splice
    "t/string-test.neon",           # string
    "t/strings.neon",               # string__splice
    "t/struct-test.neon",           # CALLMF
    "t/sudoku-test.neon",           # import
    "t/sys-exit.neon",              # sys
    "t/time-stopwatch.neon",        # CALLMF
    "t/time-test.neon",             # import
    "t/unicode-char.neon",          # UTF-8
    "t/unicode-length.neon",        # UTF-8
    "t/variant-test.neon",          # variant module
    "t/win32-test.neon",            # CALLMF
]

fullname = sys.argv[1]
path, name = os.path.split(fullname)

if fullname.replace("\\", "/") in ExcludeTests:
    sys.exit(99)

if fullname.startswith("lib/") or fullname.startswith("lib\\"):
    sys.exit(99)

subprocess.check_call([os.path.join("bin", "neonc"), "-q", fullname])
subprocess.check_call(["java", "-enableassertions", "-cp", "exec/jnex/src", "org.neon_lang.jnex.Executor", fullname + "x"])
