cmake_minimum_required(VERSION 3.10)
Include(FetchContent)

project(NoarrExamples
  VERSION 0.0.1
  DESCRIPTION "Examples for Noarr usage"
  LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

FetchContent_Declare(
  Noarr
  GIT_REPOSITORY https://github.com/jiriklepl/noarr-structures.git
  GIT_TAG        main)
FetchContent_MakeAvailable(Noarr)

include_directories(${Noarr_SOURCE_DIR}/include)
include_directories(include)

add_executable(noarr-2d-jacobi 2d-jacobi/main.cpp)
add_executable(noarr-floyd-warshall floyd-warshall/main.cpp)
add_executable(noarr-gemm gemm/main.cpp)
add_executable(noarr-gram-schmidt gram-schmidt/main.cpp)
