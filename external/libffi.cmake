#if (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
#    enable_language(ASM)
#    set(ffi_platform
#        libffi-3.2.1/src/x86/darwin64.S
#        libffi-3.2.1/src/x86/ffi64.c
#    )
#elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
#    enable_language(ASM)
#    set(ffi_platform
#        libffi-3.2.1/src/x86/ffi64.c
#        libffi-3.2.1/src/x86/unix64.S
#    )
#elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
#    enable_language(ASM_MASM)
#    set(ffi_platform
#        libffi-3.2.1/x86-win64/win62.asm
#    )
#endif ()
#add_library(ffi
#    libffi-3.2.1/src/closures.c
#    libffi-3.2.1/src/x86/ffi.c
#    libffi-3.2.1/src/prep_cif.c
#    libffi-3.2.1/src/types.c
#    ${ffi_platform}
#)
#target_compile_options(ffi PUBLIC -DFFI_BUILDING)
#if (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
#    target_include_directories(ffi
#        PRIVATE libffi-3.2.1/include
#        PUBLIC lib/libffi-3.2.1/include
#    )
#elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
#    target_include_directories(ffi
#        PRIVATE libffi-3.2.1/include
#        PUBLIC lib/libffi-3.2.1/include
#    )
#elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
#    target_include_directories(ffi
#        PRIVATE libffi-3.2.1/include
#        PUBLIC lib/libffi-3.2.1/include
#    )
#endif ()
find_library(
    ffi
    NAMES ffi libffi
    PATHS lib
)
if (NOT ffi)
    #message(FATAL_ERROR "Could not find ffi library, run 'scons external/lib' first")
    #if (NOT EXISTS libffi-3.2.1)
    #    execute_process(
    #        COMMAND python ../scripts/extract.py libffi-3.2.1.tar.gz .
    #        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    #        RESULT_VARIABLE retcode
    #    )
    #    if (NOT "${retcode}" STREQUAL "0")
    #        message(FATAL_ERROR "Fatal error extracting archive")
    #    endif ()
    #endif ()
    #if (WIN32)
    #else (WIN32)
    #    add_custom_command(
    #        OUTPUT lib/libffi.a
    #        COMMAND cd libffi-3.2.1 && ./configure --prefix=`pwd`/.. && make && make install
    #    )
    #    add_custom_target(libffi DEPENDS lib/libffi.a)
    #    add_library(ffi STATIC IMPORTED GLOBAL)
    #    add_dependencies(ffi libffi)
    #    set_target_properties(ffi PROPERTIES IMPORTED_LOCATION external/lib/libffi.a)
    #endif (WIN32)
endif ()
