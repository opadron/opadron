
cmake_minimum_required(VERSION 2.6)

enable_testing()
project(code C)

add_library(code id_allocator.c mem_stack.c tic.c trie.c)
include_directories("${PROJECT_SOURCE_DIR}")

add_executable(hello hello.c)
add_executable(goodbye goodbye.c)

add_subdirectory("tests")

