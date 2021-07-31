file(GLOB headers "*.hpp")
file(GLOB code "*.cpp")

add_subdirectory(concurrent)
add_subdirectory(io)
add_subdirectory(math)

set(target "utils")
add_library(${target} STATIC ${headers} ${code})

target_link_libraries(${target}
    data
)

target_include_directories(${target}
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/..
)