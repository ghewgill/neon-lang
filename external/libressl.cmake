if (OPENSSL_FOUND)
    set(OPENSSL_TARGET "OpenSSL::SSL;OpenSSL::Crypto")
else ()
    if (NOT EXISTS external/libressl-3.2.2)
        execute_process(
            COMMAND python3 ../scripts/extract.py libressl-3.2.2.tar.gz .
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            RESULT_VARIABLE retcode
        )
        if (NOT "${retcode}" STREQUAL "0")
            message(FATAL_ERROR "Fatal error extracting archive")
        endif ()
    endif ()

    if (WIN32)
        # TODO
    else (WIN32)
        add_custom_command(
            OUTPUT lib/libssl.a lib/libcrypto.a
            COMMAND cd libressl-3.2.2 && ./configure --prefix=`pwd`/.. --with-pic && make && make install
        )
        add_custom_target(libssl DEPENDS lib/libssl.a)
        add_library(RESSL::SSL STATIC IMPORTED GLOBAL)
        add_dependencies(RESSL::SSL libssl)
        set_target_properties(RESSL::SSL PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/include")
        set_target_properties(RESSL::SSL PROPERTIES IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/lib/libssl.a")
        add_custom_target(libcrypto DEPENDS lib/libcrypto.a)
        add_library(RESSL::Crypto STATIC IMPORTED GLOBAL)
        add_dependencies(RESSL::Crypto libcrypto)
        set_target_properties(RESSL::Crypto PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/include")
        set_target_properties(RESSL::Crypto PROPERTIES IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/lib/libcrypto.a")
        set(OPENSSL_TARGET "RESSL::SSL;RESSL::Crypto")
    endif (WIN32)
endif ()
set(OPENSSL_TARGET "${OPENSSL_TARGET}" PARENT_SCOPE)
