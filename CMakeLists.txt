cmake_minimum_required(VERSION 3.20)

project(kaleidoscope LANGUAGES CXX)

add_compile_options(-Wall -Wextra)

file(GLOB SRC_FILES src/*.cpp)

add_executable(main ${SRC_FILES})
