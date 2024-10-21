cmake_minimum_required(VERSION 3.20.0)
project(smartt VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 20)
add_executable(smartt main.cpp)
target_compile_options(${PROJECT_NAME} PRIVATE -O0 -Wall)

