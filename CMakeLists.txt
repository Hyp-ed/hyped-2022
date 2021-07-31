cmake_minimum_required(VERSION 3.21.0 FATAL_ERROR)

set_source_files_properties(arch PROPERTIES GENERATED true)
add_custom_command (
    OUTPUT arch
    COMMAND sh -c "if [[ $(uname -m) == *64* ]]; then echo 64; else echo 32; fi"
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)
set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O2 -pthread")
set(CMAKE_CXX_LINKER_FLAGS "${CMAKE_CXX_LINKER_FLAGS} -Wall -Wextra -stdlib=libc++ -lpthread")
add_compile_definitions(ARCH_64)
add_compile_definitions(LINUX)

include(${CMAKE_SOURCE_DIR}/lib/rapidjson.cmake)
include(${CMAKE_SOURCE_DIR}/lib/eigen.cmake)

set(project hyped)
project(${project})

include_directories(
    ${RAPIDJSON_INCLUDE_DIR}
    ${EIGEN_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/src
)

add_subdirectory(${CMAKE_SOURCE_DIR}/src/data)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/embrakes)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/navigation)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/propulsion)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/sensors)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/state_machine)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/telemetry)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/utils)

set(target "hyped")
add_executable(${target} ${CMAKE_SOURCE_DIR}/run/main.cpp)

target_link_libraries(${target}
    data
    embrakes
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