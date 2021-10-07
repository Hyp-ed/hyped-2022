cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

# -------------------------------------------------- #
# LIB HELPER                                         #
# -------------------------------------------------- #

# List of all libraries for linking purposes
set(ALL_LIBS "data;brakes;navigation;propulsion;propulsion_can;sensors;state_machine;telemetry;utils;utils_concurrent;utils_io;utils_math")
function(make_lib target, include_path)
    file(GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")
    file(GLOB code "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
    add_library(${target} STATIC ${headers} ${code})
    target_include_directories(${target}
        INTERFACE ${include_path}
    )
    set(link_libs ${ALL_LIBS})
    list(REMOVE_ITEM link_libs "${target}")
    target_link_libraries(${target} ${link_libs})
    add_custom_target("${target}-format"
        COMMAND clang-format -i -style=file ${code} ${headers}
    )
    add_dependencies(${target} "${target}-format")
endfunction()


# -------------------------------------------------- #
# CONFIGURE CLANG                                    #
# -------------------------------------------------- #

set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(COVERAGE "Makes the binary produce coverage information")
if(COVERAGE)
  message("coverage:  ON")
  set(COVERAGE_FLAGS "-fprofile-instr-generate -fcoverage-mapping")
else()
  message("coverage:  OFF")
  set(COVERAGE_FLAGS "")
endif()

option(RELEASE "Configures the binary for release")
if (RELEASE)
  message("release:   ON")
  set(OPTIMISATION_FLAGS "-O2")
else()
  message("release:   OFF")
  set(OPTIMISATION_FLAGS "-Og")
endif()

option(PEDANTIC "Enable pedantic warnings" ON)
if(PEDANTIC)
  message("pedantic:  ON")
  set(WARN_FLAGS "-Wall -Wextra -Wpedantic")
else()
  message("pedantic:  OFF")
  set(WARN_FLAGS "")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread ${COVERAGE_FLAGS} ${OPTIMISATION_FLAGS} ${WARN_FLAGS}")
set(CMAKE_CXX_LINKER_FLAGS "${CMAKE_CXX_LINKER_FLAGS} -stdlib=libc++ -lpthread ${WARN_FLAGS}")

message("compile flags: ${CMAKE_CXX_FLAGS}")
message("link flags:    ${CMAKE_CXX_LINKER_FLAGS}")

add_compile_definitions(ARCH_64)
add_compile_definitions(LINUX)

# -------------------------------------------------- #
# BUILD LIBS                                         #
# -------------------------------------------------- #

# including build files for thrid party libs
include(${CMAKE_SOURCE_DIR}/lib/rapidjson.cmake)
include(${CMAKE_SOURCE_DIR}/lib/eigen.cmake)
include(${CMAKE_SOURCE_DIR}/lib/gtest.cmake)

set(project hyped)
project(${project})

# defining where the third party headers can be found
include_directories(
    ${RAPIDJSON_INCLUDE_DIR}
    ${EIGEN_INCLUDE_DIR}
    ${GTEST_INCLUDE_DIR}
)

# building all the libraries and tests
add_subdirectory(${CMAKE_SOURCE_DIR}/src/data)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/brakes)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/navigation)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/propulsion)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/sensors)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/state_machine)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/telemetry)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/utils)
add_subdirectory(${CMAKE_SOURCE_DIR}/test)

# -------------------------------------------------- #
# BUILD EXECUTABLES                                  #
# -------------------------------------------------- #

set(target "hyped")
add_executable(${target} ${CMAKE_SOURCE_DIR}/run/main.cpp)
target_link_libraries(${target}
    data
    brakes
    navigation
    propulsion
    propulsion_can
    sensors
    state_machine
    telemetry
    utils
    utils_concurrent
    utils_io
    utils_math
)
