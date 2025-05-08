cmake_minimum_required(VERSION 3.22)

project(InternetStore
  VERSION 1.0
  DESCRIPTION "Internet store RESTful server on C++ for https://github.com/Dimch84/networks-course course"
  LANGUAGES CXX)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
execute_process(
    COMMAND ${CMAKE_COMMAND} -E create_symlink
        ${CMAKE_BINARY_DIR}/compile_commands.json
        ${CMAKE_SOURCE_DIR}/compile_commands.json
)

add_subdirectory(contrib)
add_subdirectory(server)

