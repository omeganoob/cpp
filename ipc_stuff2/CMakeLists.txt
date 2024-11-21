cmake_minimum_required(VERSION 3.20.0)
project(ipc_stuff2 VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 23)
add_executable(${PROJECT_NAME})

# set(MAIN src/pipe_stuff/pipe_named_client.cpp)
# set(MAIN src/ipc_stuff/shared_mem_client.cpp)
set(MAIN src/ipc_stuff/mqueue_client.cpp)
# set(MAIN src/ipc_stuff/mqueue_client_sig.cpp)
# set(MAIN src/socket_stuff/socket_client.cpp)
# set(MAIN src/socket_stuff/hostinfo.cpp)
# set(MAIN src/dbus_stuff/add_client.cpp)
# set(MAIN src/dbus_stuff/simple_signal_catcher.cpp)

target_sources(${PROJECT_NAME} PRIVATE ${MAIN})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${DBUS_INCLUDE_DIRS})

find_package(PkgConfig REQUIRED)
pkg_check_modules(DBUS REQUIRED dbus-1)
target_link_libraries(${PROJECT_NAME} ${DBUS_LIBRARIES})
