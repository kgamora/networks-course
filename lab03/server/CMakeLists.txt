cmake_minimum_required(VERSION 3.22)

project(SockFileShare
  VERSION 1.0
  DESCRIPTION "Socket file share on C++ for https://github.com/Dimch84/networks-course course"
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SOURCES
    src/main.cpp
)

add_executable(server ${SOURCES})
