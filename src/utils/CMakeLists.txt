add_subdirectory(concurrent)
add_subdirectory(io)
add_subdirectory(math)
set(target "utils")
make_lib(${target} "${CMAKE_CURRENT_SOURCE_DIR}/..")
