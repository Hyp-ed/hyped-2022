file(GLOB headers "*.hpp")
file(GLOB code "*.cpp")

set(target "utils_concurrent")
add_library(${target} STATIC ${headers} ${code})

target_include_directories(${target}
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/../..
)
