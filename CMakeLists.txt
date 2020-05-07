# =============================================================================
# Dirac Sea Engine
# =============================================================================

cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(diracsea)

set(CMAKE_C_STANDARD 11)
set(CMAKE_BUILD_TYPE Debug)

find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

if (CMAKE_COMPILER_IS_GNUCC)
    set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -Wall -Wextra")
endif()

if (MSVC)
    set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} /W4 /wd4201 /wd4204 /WX")
endif()

add_definitions(${CMAKE_C_FLAGS})

set(INCLUDE_DIR
    source
    source/math
    )

include_directories(${INCLUDE_DIR})

set(project_SOURCES
    source/main.cpp
    )

set(project_HEADERS
    source/vector.h
    )

ADD_EXECUTABLE(DiracSea ${project_HEADERS} ${project_SOURCES})

# TODO ADD SDL
# Find the libraries that correspond to the LLVM components
# that we wish to use
# llvm_map_components_to_libnames(llvm_libs support core irreader analysis target ScalarOpts native passes mcjit)
# llvm_map_components_to_libnames(llvm_libs core native passes mcjit target)

# TODO ADD SDL 
# TARGET_LINK_LIBRARIES(necro ${llvm_libs})


execute_process (
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)
