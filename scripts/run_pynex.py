import os
import subprocess
import sys

ExcludeTests = [
    "t/array-tostring.neon",        # module
    "t/bigint-test.neon",           # module
    "t/binary-test.neon",           # module
    "t/cal-test.neon",              # os.system
    "t/cformat-test.neon",          # module
    "t/class-empty.neon",           # pointer
    "t/cmdline.neon",               # module
    "t/complex-test.neon",          # module
    "t/datetime-test.neon",         # module
    "t/debug-example.neon",         # module
    "t/debug-server.neon",          # module
    "t/decimal.neon",               # arithmetic
    "t/dictionary-sorted.neon",     # module
    "t/encoding-base64.neon",       # module
    "t/equality.neon",              # PUSHM
    "t/export-inline.neon",         # export
    "t/export-recursive.neon",      # export
    "t/export.neon",                # export
    "t/ffi.neon",                   # ffi
    "t/file-filecopied.neon",       # file
    "t/file-filecopied1.neon",      # file
    "t/file-filecopied2.neon",      # file
    "t/file-filecopied3.neon",      # file
    "t/file-symlink.neon",          # file
    "t/file-test.neon",             # file
    "t/file-writebytes.neon",       # file
    "t/file-writelines.neon",       # file
    "t/fork.neon",                  # posix
    "t/function-pointer-nowhere.neon", # functionpointer
    "t/function-pointer.neon",      # functionpointer
    "t/gc-array.neon",              # runtime
    "t/gc-long-chain.neon",         # runtime
    "t/gc1.neon",                   # runtime
    "t/gc2.neon",                   # runtime
    "t/gc3.neon",                   # runtime
    "t/import.neon",                # module
    "t/interface-parameter-export.neon", # export
    "t/interface-parameter-export2.neon", # export
    "t/interface-parameter-import.neon", # module
    "t/interface-parameter-import2.neon", # module
    "t/interface.neon",             # SWAP
    "t/intrinsic.neon",             # format
    "t/io-test.neon",               # module
    "t/json-test.neon",             # module
    "t/math-test.neon",             # precision
    "t/mkdir.neon",                 # file
    "t/mmap-test.neon",             # mmap
    "t/module-alias2.neon",         # module
    "t/module-import-name-alias2.neon", # module
    "t/module-import-name2.neon",   # module
    "t/module.neon",                # module
    "t/module2.neon",               # module
    "t/multiarray-test.neon",       # module
    "t/net-test.neon",              # net
    "t/new-init-module.neon",       # module
    "t/new-init.neon",              # io
    "t/number-ceil.neon",           # precision
    "t/os-test.neon",               # os
    "t/outer-parameter.neon",       # PUSHPOL
    "t/outer-tail.neon",            # PUSHPOL
    "t/outer.neon",                 # PUSHPOL
    "t/outer2.neon",                # PUSHPOL
    "t/parameters.neon",            # INOUT
    "t/pointer-method.neon",        # pointer
    "t/pointer-nil.neon",           # pointer
    "t/pointer-valid.neon",         # pointer
    "t/pointer.neon",               # pointer
    "t/pointer1.neon",              # pointer
    "t/pointer2.neon",              # pointer
    "t/pointer3.neon",              # pointer
    "t/pointer4.neon",              # pointer
    "t/pointer5.neon",              # pointer
    "t/pointer6.neon",              # pointer
    "t/pointer7.neon",              # pointer
    "t/pointer8.neon",              # pointer
    "t/process-test.neon",          # process
    "t/recursion-limit.neon",       # runtime
    "t/repl_import.neon",           # random
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
    "t/string-test.neon",           # module
    "t/struct-test.neon",           # module
    "t/time-stopwatch.neon",        # module
    "t/unicode-char.neon",          # utf8
    "t/valid-pointer.neon",         # pointer
    "t/variant-test.neon",          # module
    "t/while-valid.neon",           # pointer
    "t/win32-test.neon",            # module
]

fullname = sys.argv[1]
path, name = os.path.split(fullname)

if fullname.replace("\\", "/") in ExcludeTests:
    sys.exit(99)

if fullname.startswith("lib/") or fullname.startswith("lib\\"):
    sys.exit(99)

subprocess.check_call([os.path.join("bin", "neonc"), "-q", fullname])
subprocess.check_call([sys.executable, "exec/pynex/pynex.py", fullname + "x"])
