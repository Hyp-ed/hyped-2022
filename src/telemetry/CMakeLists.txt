set(target "telemetry")
make_lib(${target} "${CMAKE_CURRENT_SOURCE_DIR}/..")
add_dependencies(${target} rapidjson)
