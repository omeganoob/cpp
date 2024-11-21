cmake_minimum_required(VERSION 3.5.0)
project(my_dlib VERSION 0.1.0 LANGUAGES C CXX)

set(LIBNAME my_dlib)
set(SRC_LIB my_dlib.cpp)

add_library(${LIBNAME} SHARED)

target_sources(${LIBNAME} PRIVATE ${SRC_LIB})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

set (CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/../dll/ 
                CACHE PATH "Installation Directory" FORCE)

install(TARGETS ${LIBNAME}
                LIBRARY DESTINATION bin/)

install (DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ 
                DESTINATION include/)



