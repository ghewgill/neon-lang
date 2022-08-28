# Turn off all warnings for external code.
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    add_compile_options(/w)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
    add_compile_options(-w)
endif ()

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include)

include(libbid.cmake)
include(libhash.cmake)
include(libressl.cmake)
include(minijson.cmake)
include(zlib.cmake)
include(minizip.cmake)
include(naturaldocs.cmake)
include(pyparsing.cmake)
include(sqlite.cmake)
include(utfcpp.cmake)
