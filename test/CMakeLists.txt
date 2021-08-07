enable_testing()

set(HYPED_CONFIG_DIR "${CMAKE_SOURCE_DIR}/configurations")

# -------------------------------------------------- #
# BUILD TEST RUNNER                                  #
# -------------------------------------------------- #

# find test sources
file(GLOB_RECURSE test_sources "*.test.cpp")

# make target
set(test_target "testrunner")
set(TEST_BINARY "${CMAKE_BINARY_DIR}/test/testrunner")
add_executable(${test_target} EXCLUDE_FROM_ALL ${test_sources})
add_dependencies(${test_target} rapidjson)
add_dependencies(${test_target} eigen)
add_dependencies(${test_target} googletest)

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

add_custom_target(test
    COMMAND cp -r ${HYPED_CONFIG_DIR} ${CMAKE_BINARY_DIR}/test/
    COMMAND ${TEST_BINARY}
    DEPENDS testrunner
)

# -------------------------------------------------- #
# GENERATE COVERAGE                                  #
# -------------------------------------------------- #

if (COVERAGE)
  set(LLVM_COV_DIR "${CMAKE_CURRENT_BINARY_DIR}/coverage")
  set(LLVM_COV_RAW "${LLVM_COV_DIR}/hyped.profraw")
  set(LLVM_COV_DATA "${LLVM_COV_DIR}/hyped.profdata")
  set(LCOV_DATA "${LLVM_COV_DIR}/hyped.covdata")

  add_custom_target(coverage
      COMMAND mkdir -p ${LLVM_COV_DIR}
      COMMAND sh -c "LLVM_PROFILE_FILE=\"${LLVM_COV_RAW}\" ${TEST_BINARY}"
      COMMAND llvm-profdata merge -sparse ${LLVM_COV_RAW} -o ${LLVM_COV_DATA}
      COMMAND llvm-cov export -format=lcov -instr-profile=${LLVM_COV_DATA} ${TEST_BINARY} > ${LCOV_DATA}
      COMMAND genhtml --quiet --output-directory ${LLVM_COV_DIR}/html ${LCOV_DATA}
      DEPENDS testrunner
  )
endif()
