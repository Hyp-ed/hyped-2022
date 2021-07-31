file(GLOB headers "*.hpp")
file(GLOB code "*.cpp")

set(target "telemetry")
add_library(${target} STATIC ${headers} ${code})
add_dependencies(${target} rapidjson)

target_link_libraries(${target}
    utils
    data
)

target_include_directories(${target}
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/..
)