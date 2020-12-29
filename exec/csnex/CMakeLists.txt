set(CSNEX
    csnex.cs
)
add_custom_command(
    OUTPUT csnex.exe
    COMMAND ${CSC} ${CSNEX}
    DEPENDS ${CSNEX}
)
add_custom_target(csnex_target ALL
    DEPENDS csnex.exe
)
