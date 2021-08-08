include(ExternalProject)

ExternalProject_Add(
    rapidjson
    PREFIX "lib/rapidjson"
    GIT_REPOSITORY "https://github.com/Tencent/rapidjson.git"
    TIMEOUT 10
    CMAKE_ARGS
        -DRAPIDJSON_BUILD_TESTS=OFF
        -DRAPIDJSON_BUILD_DOC=OFF
        -DRAPIDJSON_BUILD_EXAMPLES=OFF
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    UPDATE_COMMAND ""
)

# Prepare RapidJSON (RapidJSON is a header-only library)
ExternalProject_Get_Property(rapidjson source_dir)
set(RAPIDJSON_INCLUDE_DIR ${source_dir}/include)
set_target_properties(rapidjson PROPERTIES EXCLUDE_FROM_ALL TRUE)