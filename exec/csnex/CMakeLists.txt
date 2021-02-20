set(CSNEX_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/csnex.cs
    ${CMAKE_CURRENT_SOURCE_DIR}/Bytecode.cs
    ${CMAKE_CURRENT_SOURCE_DIR}/Cell.cs
    ${CMAKE_CURRENT_SOURCE_DIR}/Enums.cs
    ${CMAKE_CURRENT_SOURCE_DIR}/Exceptions.cs
    ${CMAKE_CURRENT_SOURCE_DIR}/Exec.cs
    ${CMAKE_CURRENT_SOURCE_DIR}/Global.cs
    ${CMAKE_CURRENT_SOURCE_DIR}/Number.cs
    ${CMAKE_CURRENT_SOURCE_DIR}/Object.cs
    ${CMAKE_CURRENT_SOURCE_DIR}/Opcode.cs
)

set(CSNEX_RUNTIME
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/runtime.cs
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/sys.cs
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/textio.cs
)

add_custom_command(
    OUTPUT csnex.exe
    COMMAND ${CSC} -warn:4 -warnaserror+ -debug+ -d:DEBUG -appconfig:App.config -out:csnex.exe ${CSNEX_SOURCES} ${CSNEX_RUNTIME}
    DEPENDS ${CSNEX_SOURCES}
)

add_custom_target(csnex_target ALL
    DEPENDS csnex.exe
)
