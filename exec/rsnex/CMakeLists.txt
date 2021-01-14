find_program(RUSTC rustc)

add_custom_command(
    OUTPUT rsnex${CMAKE_EXECUTABLE_SUFFIX}
    COMMAND ${RUSTC} -o rsnex${CMAKE_EXECUTABLE_SUFFIX} rsnex.rs
    DEPENDS rsnex.rs
)

add_custom_target(rsnex_target ALL
    DEPENDS rsnex${CMAKE_EXECUTABLE_SUFFIX}
)
