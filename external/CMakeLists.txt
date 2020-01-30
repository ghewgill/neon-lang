# Compile options from the top CMakeLists.txt file are all inherited,
# including the -Werror option. So add -Wno-error here to disable errors
# on warnings for external code.
add_compile_options(-Wno-error)

include(libbid.cmake)
include(libgmp.cmake)
include(libhash.cmake)
include(minijson.cmake)
include(minizip.cmake)
include(naturaldocs.cmake)
include(pyparsing.cmake)
include(sqlite.cmake)
include(utfcpp.cmake)
include(zlib.cmake)
