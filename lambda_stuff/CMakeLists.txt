cmake_minimum_required(VERSION 3.10.0)
project(lambda VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 20)
add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE main.c++)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_options(${PROJECT_NAME} PUBLIC -Wall -fno-elide-constructors)