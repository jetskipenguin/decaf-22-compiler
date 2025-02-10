cmake_minimum_required(VERSION 3.18)
project(decafe-22-compiler)

set(CMAKE_CXX_STANDARD 20)

find_package(Boost REQUIRED COMPONENTS regex)
add_executable(decafe-22-compiler src/main.cpp)
target_link_libraries(decafe-22-compiler PRIVATE Boost::regex)
