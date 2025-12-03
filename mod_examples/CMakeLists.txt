cmake_minimum_required(VERSION 3.10)
project(my_first_mod)

set(CMAKE_CXX_STANDARD 17)
set(ENGINE_SRC_DIR "${CMAKE_SOURCE_DIR}/../src")

add_library(my_first_mod SHARED my_first_mod.cpp)
target_include_directories(my_first_mod PRIVATE ${ENGINE_SRC_DIR})