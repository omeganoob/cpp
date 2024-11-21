cmake_minimum_required(VERSION 3.10.0)
project(test VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 23)
add_executable(${PROJECT_NAME})

# set(MAIN main.cpp)
set(MAIN server_main.cpp)

target_sources(${PROJECT_NAME} PRIVATE 
                                ${MAIN})

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/MQIPC)

target_link_libraries(${PROJECT_NAME} PUBLIC MQIPC)

target_include_directories(${PROJECT_NAME} PUBLIC 
                                            ${PROJECT_BINARY_DIR}/MQIPC)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
