cmake_minimum_required(VERSION 3.10.0)

project(IPC_project)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(ipc_lib
    src/ipc_process.cpp
    src/server_queue.cpp
    src/communication.cpp
)

add_executable(server main/server_main.cpp)
add_executable(client main/client_main.cpp)
add_executable(service main/service_main.cpp)

target_link_libraries(server ipc_lib)
target_link_libraries(service ipc_lib)
target_link_libraries(client ipc_lib)
