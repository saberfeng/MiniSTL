cmake_minimum_required(VERSION 3.7)
project(MiniSTL)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp allocator.h)
add_executable(MiniSTL ${SOURCE_FILES})