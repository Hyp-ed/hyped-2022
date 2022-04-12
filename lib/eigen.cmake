include(ExternalProject)

ExternalProject_Add(
    eigen
    PREFIX "lib/eigen"
    GIT_REPOSITORY "https://gitlab.com/libeigen/eigen"
    TIMEOUT 10
    CMAKE_ARGS ""
    CONFIGURE_COMMAND ""
    GIT_TAG "0fd6b4f7" # release 3.3.4
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    UPDATE_COMMAND ""
)

ExternalProject_Get_Property(eigen source_dir)
set(EIGEN_INCLUDE_DIR ${source_dir}/)
set_target_properties(eigen PROPERTIES EXCLUDE_FROM_ALL TRUE)