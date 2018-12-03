find_program(RUSTC rustc)

add_custom_command(
    OUTPUT rsnex
    COMMAND ${RUSTC} -o rsnex rsnex.rs
    DEPENDS rsnex.rs
)

add_custom_target(rsnex_target ALL
    DEPENDS rsnex
)
