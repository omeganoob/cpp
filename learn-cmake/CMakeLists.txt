cmake_minimum_required(VERSION 3.19)
project("cmake_test")
add_executable(${PROJECT_NAME})
add_subdirectory(folder_a)
target_sources(${PROJECT_NAME} PRIVATE main.cpp)