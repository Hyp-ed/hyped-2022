file(GLOB_RECURSE test_sources "*.cpp")

set(test_target "hyped_test")
add_executable(${test_target} ${test_sources})
add_dependencies(${test_target} rapidjson)
add_dependencies(${test_target} eigen)
add_dependencies(${test_target} googletest)

include_directories(${GTEST_INCLUDE_DIR})

target_link_libraries(${test_target}
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
    gtest
)