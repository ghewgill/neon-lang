if (WIN32)
    # RUNTIME_OUTPUT_DIRECTORY is where msbuild puts dlls.
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG .)
    add_library(neon_win32 SHARED
        win32.cpp
    )
    target_include_directories(neon_win32 PRIVATE ../../common)
endif ()
