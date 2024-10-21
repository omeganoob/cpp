cmake_minimum_required(VERSION 3.20.0)

project(lib_test VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)

add_executable(${PROJECT_NAME})

set(MAIN main.cpp)

target_sources(${PROJECT_NAME} PRIVATE ${MAIN})

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/my_math_lib)

target_link_libraries(${PROJECT_NAME} PUBLIC my_math_lib)

target_compile_options(${PROJECT_NAME} PUBLIC -Wall -fno-elide-constructors)