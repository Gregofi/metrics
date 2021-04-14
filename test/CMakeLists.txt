cmake_minimum_required(VERSION 3.16)

find_package(LLVM REQUIRED CONFIG)
find_package(Clang REQUIRED CONFIG)

file(GLOB tests *.cpp)

foreach(testsrc ${tests})
    get_filename_component(file ${testsrc} NAME_WE)
    add_executable(${file} ${file}.cpp ${PROJECT_SOURCE_DIR}/src/${file}Visitor.cpp ${PROJECT_SOURCE_DIR}/src/Utility.cpp)
    target_link_libraries(${file} PRIVATE LLVM clang-cpp)
    add_test(NAME ${file} COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${file})
endforeach()

