file(GLOB headers "*.hpp")
file(GLOB code "*.cpp")

set(target "navigation")
add_library(${target} STATIC ${headers} ${code})
add_dependencies(${target} eigen)

include_directories(
    INTERFACE ${CMAKE_CURRENT_DIR}/..
)