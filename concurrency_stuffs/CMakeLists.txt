cmake_minimum_required(VERSION 3.10.0)
project(concurrency VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 23)
add_executable(${PROJECT_NAME})
set(MAIN asyncthing.cpp)
# set(MAIN main.cpp)
# set(MAIN numberthread.cpp)
# set(MAIN ownthread.cpp)
# set(MAIN readthread.cpp)
target_sources(${PROJECT_NAME} PRIVATE ${MAIN})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

target_include_directories(${PROJECT_NAME} PUBLIC "${PROJECT_BINARY_DIR}")

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/stop_watch)

target_link_libraries(${PROJECT_NAME} PUBLIC stopwatch)

target_compile_options(${PROJECT_NAME} PUBLIC -Wall -fno-elide-constructors)