cmake_minimum_required(VERSION 3.16)

set(CMAKE_C_COMPILER "clang")
set(CMAKE_CXX_COMPILER "/usr/bin/clang++")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

project(metrics_project)
include(CTest)

find_package(LLVM REQUIRED CONFIG)
find_package(Clang REQUIRED CONFIG)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

set(CMAKE_CXX_STANDARD 17)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I${PROJECT_SOURCE_DIR} -I${LLVM_INCLUDE_DIRS}")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

add_executable(metrics main.cpp src/FuncInfoVisitor.cpp src/CyclomaticVisitor.cpp src/MetricVisitor.cpp
        src/HalsteadVisitor.cpp src/NPathVisitor.cpp src/FansVisitor.cpp src/ClassOverviewVisitor.cpp src/Utility.cpp)

target_link_libraries(metrics
        PRIVATE
        LLVM
        clang-cpp
)

message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
enable_testing()
add_subdirectory(test)

