# =============================================================================
# Dirac Sea Engine
# =============================================================================

cmake_minimum_required(VERSION 3.17 FATAL_ERROR)
project(DiracSea)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Debug)

# TODO ADD SDL
# find_package(LLVM REQUIRED CONFIG)
# message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
# message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

# include_directories(${LLVM_INCLUDE_DIRS})
# add_definitions(${LLVM_DEFINITIONS})

# C++ compiler flags
if (CMAKE_COMPILER_IS_GNUCC)
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif()

if (MSVC)
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /W4 /wd4201 /wd4204 /WX")
endif()

add_definitions(${CMAKE_CXX_FLAGS})

# SSE support

set(INCLUDE_DIR
    source
    source/math
    source/tests
    source/tests/math
    source/tests/math/vector
    )

include_directories(${INCLUDE_DIR})

set(project_SOURCES
    source/main.cpp
    source/math/coordinate_system.cpp
    source/tests/tests.cpp
    source/tests/test_framework.cpp
    source/tests/math/vector/vector_tests.cpp
    source/tests/math/matrix/matrix_tests.cpp
    )

set(project_HEADERS
    source/math/types.h
    source/math/matrix22.h
    source/math/matrix33.h
    source/math/matrix44.h
    source/math/vector.h
    source/math/coordinate_system.h
    source/tests/tests.h
    source/tests/test_framework.h
    source/tests/math/vector/vector_tests.h
    source/tests/math/matrix/matrix_tests.h
    )

ADD_EXECUTABLE(DiracSea ${project_HEADERS} ${project_SOURCES})

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT DiracSea)

# TODO ADD SDL
# Find the libraries that correspond to the LLVM components
# that we wish to use
# llvm_map_components_to_libnames(llvm_libs support core irreader analysis target ScalarOpts native passes mcjit)
# llvm_map_components_to_libnames(llvm_libs core native passes mcjit target)

# TODO ADD SDL 
# TARGET_LINK_LIBRARIES(necro ${llvm_libs})

