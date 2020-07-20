cmake_minimum_required(VERSION 3.0)
cmake_policy(SET CMP0054 NEW)
project(neon_win32)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (WIN32)
    # RUNTIME_OUTPUT_DIRECTORY is where msbuild puts dlls.
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG .)
    add_library(neon_win32 SHARED
        win32.cpp
    )
    target_include_directories(neon_win32 PRIVATE ../../common)
endif ()
