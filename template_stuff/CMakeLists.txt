cmake_minimum_required(VERSION 3.5.0)
project(template_stuff VERSION 0.1.0 LANGUAGES C CXX)
set (CMAKE_CXX_STANDARD 11)
# set(CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED TRUE)
add_executable(template_stuff main.cpp)
target_compile_options(${PROJECT_NAME} PRIVATE -fno-elide-constructors)


