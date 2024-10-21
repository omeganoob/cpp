cmake_minimum_required(VERSION 3.19)
project(test_oop)
add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE main.cpp calc.cpp)