cmake_minimum_required(VERSION 3.20)

project(kaleidoscope LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(LLVM_DIR "$ENV{HOME}/llvm-install/lib/cmake/llvm")
find_package(LLVM REQUIRED CONFIG)

set(MLIR_DIR "$ENV{HOME}/llvm-install/lib/cmake/mlir")
find_package(MLIR REQUIRED CONFIG)

include_directories(SYSTEM ${LLVM_INCLUDE_DIRS})
include_directories(SYSTEM ${MLIR_INCLUDE_DIRS})

separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

add_compile_options(-Wall -Wextra -g)

file(GLOB SRC_FILES src/*.cpp)

add_executable(main ${SRC_FILES})

llvm_map_components_to_libnames(llvm_libs support core irreader)

target_link_libraries(main
    PRIVATE
    MLIRSupport
    MLIRIR
    MLIRParser
    MLIRArithDialect
    MLIRFuncDialect

    ${llvm_libs}
    LLVMSupport
)
