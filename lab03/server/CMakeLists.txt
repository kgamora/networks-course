cmake_minimum_required(VERSION 3.22)

project(SockFileShare
  VERSION 1.0
  DESCRIPTION "Socket file share on C++ for https://github.com/Dimch84/networks-course course"
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

execute_process(
    COMMAND ${CMAKE_COMMAND} -E create_symlink
        ${CMAKE_BINARY_DIR}/compile_commands.json
        ${CMAKE_SOURCE_DIR}/compile_commands.json
)

add_subdirectory(contrib)

find_package(httpparser)

set(SOURCES
    src/main.cpp
    src/library/socket.cpp
    src/library/session.cpp
)

add_executable(server ${SOURCES})

target_link_libraries(server 
  PUBLIC httpparser)
