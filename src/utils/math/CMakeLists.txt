file(GLOB headers "*.hpp")
file(GLOB code "*.cpp")

set(target "utils_math")
add_library(${target} STATIC ${headers} ${code})
add_dependencies(${target} eigen)

target_include_directories(${target}
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/../..
)