cmake_minimum_required(VERSION 3.20)
project(MQIPC VERSION 0.1.0 LANGUAGES C CXX)

set(LIBNAME MQIPC)

set(SRC_IPC_SERVER ${CMAKE_CURRENT_SOURCE_DIR}/src/ipc_server.cpp)
set(SRC_IPC_APP ${CMAKE_CURRENT_SOURCE_DIR}/src/ipc_app.cpp)
set(SRC_IPC_MSG ${CMAKE_CURRENT_SOURCE_DIR}/src/message.cpp)
set(SRC_IPC_TCOLOR ${CMAKE_CURRENT_SOURCE_DIR}/src/t_color.cpp)
set(SRC_QUEUE_HANDLER ${CMAKE_CURRENT_SOURCE_DIR}/src/queue_handler.cpp)

add_library(${LIBNAME} SHARED)

target_sources(${LIBNAME} PRIVATE 
                            ${SRC_IPC_SERVER}
                            ${SRC_IPC_APP}
                            ${SRC_IPC_MSG}
                            ${SRC_IPC_TCOLOR}
                            ${SRC_QUEUE_HANDLER})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

set(CMAKE_INSTALL_PREFIX "/usr/local/" CACHE PATH "Installation Directory" FORCE)

install(TARGETS ${LIBNAME}
    LIBRARY DESTINATION  lib/${LIBNAME}
)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/ DESTINATION include)