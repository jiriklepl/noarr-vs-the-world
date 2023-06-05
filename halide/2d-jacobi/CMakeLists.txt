cmake_minimum_required(VERSION 3.10)
Include(FetchContent)

# set the project name
project(2Djacobi VERSION 0.0.1)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# setup the test runner executable
file(GLOB_RECURSE tests_source_files CONFIGURE_DEPENDS "src/*.cpp" )

add_executable(2d-jacobi ${tests_source_files})
target_include_directories(2d-jacobi PRIVATE include)

# ask the compiler to print maximum warnings
if(MSVC)
  target_compile_options(2d-jacobi PRIVATE /W4)
else()
  target_compile_options(2d-jacobi PRIVATE -Wall -Wextra -pedantic)
endif()
