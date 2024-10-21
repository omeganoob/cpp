cmake_minimum_required(VERSION 3.20.0)
project(smart_pointer VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 20)
# set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED true)
add_executable(smart_pointer main.cpp)
target_compile_options(${PROJECT_NAME} PRIVATE -fno-elide-constructors)

