enable_testing()

# -------------------------------------------------- #
# BUILD TEST RUNNER                                  #
# -------------------------------------------------- #

# find test sources
file(GLOB_RECURSE test_sources "*.test.cpp")

# make target
set(test_target "hyped_test")
add_executable(${test_target} ${test_sources})
add_dependencies(${test_target} rapidjson)
add_dependencies(${test_target} eigen)
add_dependencies(${test_target} googletest)
add_dependencies(${test_target} hyped)
add_dependencies(${test_target} "${test_target}-format")

# include and link
include_directories(
    ${GTEST_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/test/src
)
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

add_custom_target("${test_target}-format"
    COMMAND clang-format -i -style=file ${test_sources}
)
add_dependencies(${test_target} "${test_target}-format")

# -------------------------------------------------- #
# GENERATE COVERAGE                                  #
# -------------------------------------------------- #

set(TEST_BINARY "${CMAKE_BINARY_DIR}/test/hyped_test")
set(LLVM_COV_DIR "${CMAKE_CURRENT_BINARY_DIR}/coverage")
set(LLVM_COV_RAW "${LLVM_COV_DIR}/hyped_test.profraw")
set(LLVM_COV_DATA "${LLVM_COV_DIR}/hyped_test.profdata")
set(LCOV_DATA "${LLVM_COV_DIR}/hyped_test.covdata")
set(HYPED_CONFIG_DIR "${CMAKE_SOURCE_DIR}/configurations")

add_custom_target(coverage
    COMMAND cp -r ${HYPED_CONFIG_DIR} ${CMAKE_BINARY_DIR}/test/
    COMMAND mkdir -p ${LLVM_COV_DIR}
    COMMAND sh -c "LLVM_PROFILE_FILE=\"${LLVM_COV_RAW}\" ${TEST_BINARY}"
    COMMAND llvm-profdata merge -sparse ${LLVM_COV_RAW} -o ${LLVM_COV_DATA}
    COMMAND llvm-cov export -format=lcov -instr-profile=${LLVM_COV_DATA} ${TEST_BINARY} > ${LCOV_DATA}
    COMMAND genhtml --quiet --output-directory ${LLVM_COV_DIR}/html ${LCOV_DATA}
    DEPENDS hyped_test
)
