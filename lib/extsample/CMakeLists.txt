cmake_minimum_required(VERSION 3.0)
project(neon_extsample)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY .)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG .)

add_library(neon_extsample SHARED extsample.c)
