find_program(RUSTC rustc)

set(RSNEX_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/rsnex.rs
)

add_custom_command(
    OUTPUT rsnex${CMAKE_EXECUTABLE_SUFFIX}
    COMMAND ${RUSTC} -o rsnex${CMAKE_EXECUTABLE_SUFFIX} ${RSNEX_SOURCES}
    DEPENDS ${RSNEX_SOURCES}
)

add_custom_target(rsnex_target ALL
    DEPENDS rsnex${CMAKE_EXECUTABLE_SUFFIX}
)
