cmake_minimum_required(VERSION 2.8.9)

project(ZCompressor)

if (CMAKE_COMPILER_IS_GNUCXX)
    add_compile_options(-std=c++11)
endif()

find_package(Qt5 REQUIRED Core)

add_subdirectory(compressor)
add_subdirectory(zcompressor)
