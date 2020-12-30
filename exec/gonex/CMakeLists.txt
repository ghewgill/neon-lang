find_program(GO go)

set(GONEX_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/gonex.go
)

add_custom_command(
    OUTPUT gonex
    COMMAND ${GO} build ${GONEX_SOURCES}
    DEPENDS ${GONEX_SOURCES}
)

add_custom_target(gonex_target ALL
    DEPENDS gonex
)
