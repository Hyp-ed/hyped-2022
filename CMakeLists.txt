cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

# -------------------------------------------------- #
# LIB HELPER                                         #
# -------------------------------------------------- #

# List of all libraries for linking purposes
set(ALL_LIBS "data;brakes;navigation;propulsion;propulsion_can;sensors;state_machine;demo_state_machine;telemetry;utils;utils_concurrent;utils_io;utils_math")
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
# CONFIGURE COMPILER                                 #
# -------------------------------------------------- #

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
  set(WARN_FLAGS "-Werror -Wextra -Wpedantic")
else()
  message("pedantic:  OFF")
  set(WARN_FLAGS "")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread ${COVERAGE_FLAGS} ${OPTIMISATION_FLAGS} ${WARN_FLAGS}")
set(CMAKE_CXX_LINKER_FLAGS "${CMAKE_CXX_LINKER_FLAGS} -stdlib=libc++ -lpthread ${WARN_FLAGS}")


# -------------------------------------------------- #
# GET CROSS COMPILER                                 #
# -------------------------------------------------- #

option(CROSS "Cross compile for the BBB")
if(CROSS)
  message("cross:     ON")
  set(CMAKE_SYSTEM_NAME "Linux")
  if (CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
    message("Cross compiling from x86 Linux to BBB")
    set(CC "arm-none-linux-gnueabihf")
    set(CC_DIRECTORY "${CMAKE_BINARY_DIR}/gcc-arm-10.3-2021.07-x86_64-${CC}")
    set(CMAKE_CXX_COMPILER "${CC_DIRECTORY}/bin/${CC}-c++")
    set(CMAKE_CXX_LINKER_FLAGS "${CMAKE_CXX_LINKER_FLAGS} -static")
  elseif(CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin")
    message(FATAL_ERROR "Cross compilation is not supported for MacOS")
  endif()
else()
  message("cross:     OFF")
  set(CMAKE_SYSTEM_NAME "${CMAKE_HOST_SYSTEM_NAME}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DARCH_64")
endif()

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DLINUX")
endif()

message("host system:    ${CMAKE_HOST_SYSTEM_NAME}")
message("target system:  ${CMAKE_SYSTEM_NAME}")
message("compile flags: ${CMAKE_CXX_FLAGS}")
message("link flags:    ${CMAKE_CXX_LINKER_FLAGS}")
message("compiler:       ${CMAKE_CXX_COMPILER}")


# -------------------------------------------------- #
# BUILD LIBS                                         #
# -------------------------------------------------- #

# including build files for thrid party libs
include(${CMAKE_SOURCE_DIR}/lib/rapidjson.cmake)
include(${CMAKE_SOURCE_DIR}/lib/eigen.cmake)
include(${CMAKE_SOURCE_DIR}/lib/gtest.cmake)

set(project hyped)
project(${project} CXX)

# defining where the third party headers can be found
include_directories(
    ${RAPIDJSON_INCLUDE_DIR}
    ${EIGEN_INCLUDE_DIR}
    ${GTEST_INCLUDE_DIR}
)

# building all the libraries and tests
add_subdirectory(${CMAKE_SOURCE_DIR}/src)


if (CROSS)
else()
  add_subdirectory(${CMAKE_SOURCE_DIR}/test)
endif()
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

set(target "hyped_demo")
add_executable(${target} ${CMAKE_SOURCE_DIR}/run/demo.cpp)
target_link_libraries(${target}
    data
    brakes
    navigation
    propulsion
    propulsion_can
    sensors
    demo_state_machine
    telemetry
    utils
    utils_concurrent
    utils_io
    utils_math
)

set(target "debugger")
add_executable(${target} ${CMAKE_SOURCE_DIR}/run/debugger.cpp)
target_link_libraries(${target}
    data
    debugging
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
