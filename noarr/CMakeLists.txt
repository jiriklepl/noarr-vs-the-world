cmake_minimum_required(VERSION 3.10)

project(NoarrExamples)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)


file(GLOB example_dirs RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *)

foreach(example_dir ${example_dirs})
    if (IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${example_dir}
        AND NOT ${example_dir} STREQUAL "build"
        AND EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${example_dir}/CMakeLists.txt)
        message(STATUS "Adding example ${example_dir}")
        add_subdirectory(${example_dir})
    else()
        list(REMOVE_ITEM example_dirs ${example_dir})
    endif()
endforeach()
