cmake_minimum_required(VERSION 3.10)

project(HalideExamples
    VERSION 0.0.1
    DESCRIPTION "Examples of Halide usage"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)


file(GLOB example_dirs RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *)

find_package(Halide REQUIRED)

link_libraries(Halide::Halide)
include_directories(include)

add_executable(halide-2d-jacobi 2d-jacobi/main.cpp)
add_executable(halide-floyd-warshall floyd-warshall/main.cpp)
add_executable(halide-gemm gemm/main.cpp)
add_executable(halide-gram-schmidt gram-schmidt/main.cpp)
