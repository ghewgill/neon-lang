find_program(GO go)

add_custom_command(
    OUTPUT gonex${CMAKE_EXECUTABLE_SUFFIX}
    COMMAND ${GO} build
    DEPENDS gonex.go
)

add_custom_target(gonex_target ALL
    DEPENDS gonex${CMAKE_EXECUTABLE_SUFFIX}
)
