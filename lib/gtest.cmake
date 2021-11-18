include(ExternalProject)

ExternalProject_Add(
    googletest
    PREFIX "lib/gtest"
    GIT_REPOSITORY "https://github.com/google/googletest"
    GIT_TAG "e2239ee6043f73722e7aa812a459f54a28552929" # release 1.11.0 
    TIMEOUT 10
    INSTALL_COMMAND ""
)

ExternalProject_Get_Property(googletest source_dir)
set(GTEST_INCLUDE_DIR ${source_dir}/googletest/include)
set_target_properties(googletest PROPERTIES EXCLUDE_FROM_ALL TRUE)

ExternalProject_Get_Property(googletest binary_dir)
set(GTEST_LIBRARY_PATH ${binary_dir}/lib/libgtest.a)
set(GTEST_LIBRARY gtest)
add_library(${GTEST_LIBRARY} UNKNOWN IMPORTED)
set_property(TARGET ${GTEST_LIBRARY} PROPERTY IMPORTED_LOCATION
                ${GTEST_LIBRARY_PATH} )
add_dependencies(${GTEST_LIBRARY} googletest)
set_target_properties(gtest PROPERTIES EXCLUDE_FROM_ALL TRUE)