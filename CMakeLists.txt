cmake_minimum_required(VERSION 3.16)

set(CMAKE_C_COMPILER "clang")
set(CMAKE_CXX_COMPILER "/usr/bin/clang++")

project(metrics)

find_package(LLVM REQUIRED CONFIG)
find_package(Clang REQUIRED CONFIG)

include("${CLANG_CMAKE_DIR}/ClangConfig.cmake")
include("${LLVM_CMAKE_DIR}/LLVMConfig.cmake")

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

set(CMAKE_CXX_STANDARD 17)

set(LLVM_LINK_COMPONENTS support)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -I${PROJECT_SOURCE_DIR}")

add_executable(metrics main.cpp src/FuncInfoVisitor.cpp src/CyclomaticVisitor.cpp src/MetricVisitor.cpp src/HalsteadVisitor.cpp src/NPathVisitor.cpp include/NPathVisitor.hpp include/Logging.hpp)

target_link_libraries(metrics
        PRIVATE
        LLVM
        clang-cpp
)

enable_testing()
add_subdirectory( tests )

