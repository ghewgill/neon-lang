find_program(GO go)

add_custom_command(
    OUTPUT gonex
    COMMAND ${GO} build
    DEPENDS gonex.go
)

add_custom_target(gonex_target ALL
    DEPENDS gonex
)
