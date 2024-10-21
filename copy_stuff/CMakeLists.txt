cmake_minimum_required(VERSION 3.19.0)
project(copy_stuff VERSION 0.1.0 LANGUAGES C CXX)
set (CMAKE_CXX_STANDARD 11)
add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE main.cpp)
target_compile_options(${PROJECT_NAME} PRIVATE -fno-elide-constructors)

