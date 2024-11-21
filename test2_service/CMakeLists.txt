
cmake_minimum_required(VERSION 3.10.0)
project(my_service VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 23)
add_executable(${PROJECT_NAME})

set(MAIN service_main.cpp)

target_sources(${PROJECT_NAME} PRIVATE ${MAIN})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(${PROJECT_NAME} PUBLIC /usr/local/lib/MQIPC/libMQIPC.so)