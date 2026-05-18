# Define flags for non-standard build types.
SET(CMAKE_CXX_FLAGS_ASAN_INIT "-O1 -g -fsanitize=address")
SET(CMAKE_CXX_FLAGS_UBSAN_INIT
    "-O1 -g -fsanitize=undefined -fno-sanitize-recover=all")

cmake_minimum_required(VERSION 3.6)

project(puzzle CXX)

set(CMAKE_CXX_STANDARD 17)

if(CMAKE_CXX_COMPILER_ID MATCHES "(Clang|GNU)")
    add_compile_options(-Wall -Wextra)
endif()

add_library(solve STATIC
    src/puzzle.cpp
)

add_executable(puzzle
    src/main.cpp
)

target_link_libraries(puzzle
    PRIVATE
        solve
)

add_executable(test
    src/test.cpp
)

target_link_libraries(test
    PRIVATE
        gtest
        gtest_main
        solve
)
