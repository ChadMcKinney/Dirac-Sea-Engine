# =============================================================================
# Dirac Sea Engine
# =============================================================================

cmake_minimum_required(VERSION 3.17 FATAL_ERROR)
project(DiracSea)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/thirdparty/")
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})

find_package(Vulkan REQUIRED)
include_directories(${Vulkan_INCLUDE_DIR})

# C++ compiler flags
if (CMAKE_COMPILER_IS_GNUCC)
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif()

if (MSVC)
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /W4 /wd4201 /wd4204 /wd6255 /wd6011 /wd26812 /wd4996 /WX")
endif()

add_definitions(${CMAKE_CXX_FLAGS})

# SSE support

set(INCLUDE_DIR
    source
    source/math
    source/math/geometry
    source/platform
    source/renderer
    source/tests
    source/tests/math
    source/tests/geometry
    source/tests/math/vector
    )

include_directories(${INCLUDE_DIR})

set(project_SOURCES
    source/main.cpp
    source/math/coordinate_system.cpp
    source/platform/platform.cpp
    source/renderer/renderer.cpp
    source/tests/tests.cpp
    source/tests/test_framework.cpp
    source/tests/math/geometry/geometry_tests.cpp
    source/tests/math/quaternion/quaternion_tests.cpp
    source/tests/math/vector/vector_tests.cpp
    source/tests/math/matrix/matrix_tests.cpp
    )

set(project_HEADERS
    source/diracsea.h
    source/math/types.h
    source/math/matrix22.h
    source/math/matrix33.h
    source/math/matrix43.h
    source/math/matrix44.h
    source/math/quaternion.h
    source/math/vector2.h
    source/math/vector3.h
    source/math/vector4.h
    source/math/coordinate_system.h
    source/math/geometry/aabb.h
    source/math/geometry/line.h
    source/math/geometry/plane.h
    source/math/geometry/ray.h
    source/math/geometry/sphere.h
    source/math/geometry/triangle.h
    source/platform/platform.h
    source/renderer/renderer.h
    source/tests/tests.h
    source/tests/test_framework.h
    source/tests/math/geometry/geometry_tests.h
    source/tests/math/quaternion/quaternion_tests.h
    source/tests/math/vector/vector_tests.h
    source/tests/math/matrix/matrix_tests.h
    )

ADD_EXECUTABLE(DiracSea ${project_HEADERS} ${project_SOURCES})
target_link_libraries(DiracSea ${SDL2_LIBRARIES} ${Vulkan_LIBRARIES})

if (MSVC)
    add_custom_command(TARGET DiracSea POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${SDL2_DIR}/${VC_LIB_PATH_SUFFIX}/SDL2.dll"
        $<TARGET_FILE_DIR:DiracSea>
        )
    add_custom_command(TARGET DiracSea POST_BUILD
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/compile_shaders.bat
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Compiling shaders..."
        )
    add_custom_command(TARGET DiracSea POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/data
        $<TARGET_FILE_DIR:DiracSea>/data
        )
endif()

if (UNIX)
    add_custom_command(TARGET DiracSea POST_BUILD
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/compile_shaders.sh
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Compiling shaders..."
        )
endif()

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT DiracSea)

# Create the source groups for source tree with root at CMAKE_CURRENT_SOURCE_DIR.
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${project_HEADERS})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${project_SOURCES})
