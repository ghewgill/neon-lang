set(CSNEX_SOURCES
    csnex.cs
    Bytecode.cs
    Cell.cs
    Enums.cs
    Exceptions.cs
    Exec.cs
    Extensions.cs
    Global.cs
    Number.cs
    Object.cs
    Opcode.cs
)

set(CSNEX_RUNTIME
    lib/io.cs
    lib/random.cs
    lib/runtime.cs
    lib/string.cs
    lib/sys.cs
    lib/textio.cs
)

add_custom_command(
    OUTPUT csnex.exe
    COMMAND ${CSC} -warn:4 -warnaserror+ -debug+ -d:DEBUG -appconfig:App.config -out:csnex.exe ${CSNEX_SOURCES} ${CSNEX_RUNTIME}
    DEPENDS ${CSNEX_SOURCES}
)

add_custom_target(csnex_target ALL
    DEPENDS csnex.exe
)
