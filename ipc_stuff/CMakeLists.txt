cmake_minimum_required(VERSION 3.20.0)
project(ipc_stuff VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 23)
add_executable(${PROJECT_NAME})

# set(MAIN src/fork_stuff/main.cpp)
# set(MAIN src/fork_stuff/change_of_ppid.cpp)
# set(MAIN src/fork_stuff/wait_pid.cpp)
# set(MAIN src/pipe_stuff/pipe_basic.cpp)
# set(MAIN src/pipe_stuff/pipe_processes.cpp)
# set(MAIN src/pipe_stuff/pipe_dual.cpp)
# set(MAIN src/pipe_stuff/pipe_named_server.cpp)
# set(MAIN src/ipc_stuff/shared_mem_server.cpp)
set(MAIN src/ipc_stuff/mqueue_server.cpp)
# set(MAIN src/socket_stuff/socket_server.cpp)
# set(MAIN src/dbus_stuff/add_server.cpp)
# set(MAIN src/dbus_stuff/simple_connect.cpp)
# set(MAIN src/dbus_stuff/simple_signal.cpp)
# set(MAIN src/dbus_stuff/simple_signal_gdbus.cpp)
# set(MAIN src/dbus_stuff/media_player_service.cpp)

target_sources(${PROJECT_NAME} PRIVATE ${MAIN})

find_package(PkgConfig REQUIRED)
pkg_check_modules(DBUS REQUIRED dbus-1)
pkg_check_modules(GLIB REQUIRED glib-2.0 gio-2.0)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${DBUS_INCLUDE_DIRS})
include_directories(${GLIB_INCLUDE_DIRS})

link_directories(${GLIB_LIBRARY_DIRS})

add_definitions(${GLIB_CFLAGS_OTHER})

pkg_check_modules(GLIB REQUIRED glib-2.0 gio-2.0)
target_link_libraries(${PROJECT_NAME} ${DBUS_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${GLIB_LIBRARIES})
