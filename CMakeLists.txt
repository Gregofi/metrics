cmake_minimum_required(VERSION 3.16)
project(metrics)

set(CMAKE_CXX_STANDARD 17)

set(LLVM_LINK_COMPONENTS support)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

add_clang_executable(metrics main.cpp modules/FuncInfoVisitor.cpp)

target_link_libraries(metrics
        PRIVATE
        clangTooling
        clangBasic
        clangASTMatchers
)
