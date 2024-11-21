cmake_minimum_required(VERSION 3.5.0)
project(test VERSION 0.1.0 LANGUAGES C CXX)

add_executable(test main.cpp)

find_package(NumCpp REQUIRED)
target_link_libraries(${PROJECT_NAME} NumCpp::NumCpp)