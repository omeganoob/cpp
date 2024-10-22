cmake_minimum_required(VERSION 3.20)

set(LIBNAME my_math_lib)
set(SRC_LIB my_math_lib.cpp)

add_library(${LIBNAME})

target_sources(${LIBNAME} PRIVATE ${SRC_LIB})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

set(CMAKE_INSTALL_PREFIX "/mnt/e/fpt_fresher/cpp/lib_study/" CACHE PATH "Installation Directory" FORCE)

install(TARGETS ${LIBNAME} 
    ARCHIVE DESTINATION lib/bin/
    LIBRARY DESTINATION dll/bin/)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/ DESTINATION lib/include/)