cmake_minimum_required(VERSION 3.19.0)
project(move_stuff VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 11)
set (CMAKE_CXX_STANDARD_REQUIRED TRUE)
add_executable(move_stuff)
target_sources(${PROJECT_NAME} PRIVATE main.cpp)
target_compile_options(${PROJECT_NAME} PRIVATE -fno-elide-constructors)

