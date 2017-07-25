cmake_minimum_required(VERSION 3.7)
project(MiniSTL)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp allocator.h rb_tree.h construct.h iterator.h set.h util.h algorithm.h)
add_executable(MiniSTL ${SOURCE_FILES})