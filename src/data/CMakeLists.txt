file(GLOB headers "*.hpp")
file(GLOB code "*.cpp")

set(target "data")
add_library(${target} STATIC ${headers} ${code})

include_directories(
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/..
)