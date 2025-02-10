cmake_minimum_required(VERSION 3.18)
project(decafe-22-compiler)

set(CMAKE_CXX_STANDARD 20)

include_directories(src/third_party/boost/regex)

add_subdirectory(src/third_party/boost/regex)
add_executable(decafe-22-compiler src/main.cpp)
