cmake_minimum_required(VERSION 3.1)

project(
    blue-space
    VERSION 0.1
    LANGUAGES CXX CUDA
)

option(CUDA_MINER "Enable CUDA miner" OFF)
option(CLANG_TIDY "Run clang-tidy" OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules")

enable_testing()
find_package(GTest REQUIRED)
find_package(benchmark REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GMP REQUIRED IMPORTED_TARGET gmpxx)

if (CLANG_TIDY)
    set(CMAKE_CXX_CLANG_TIDY clang-tidy)
endif()

add_compile_options(-Wall -Wextra -pedantic)

add_subdirectory("miner-common")
add_subdirectory("miner-cpu")
if (CUDA_MINER)
    add_subdirectory("miner-cuda")
endif()
add_subdirectory("explorer")
add_subdirectory("blue-space")