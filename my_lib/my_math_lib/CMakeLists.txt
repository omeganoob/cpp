cmake_minimum_required(VERSION 3.20)

set(LIBNAME my_math_lib)
set(MY_MATH_LIB my_math_lib.cpp)

add_library(${LIBNAME} SHARED)
target_sources(${LIBNAME} PRIVATE ${MY_MATH_LIB})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

set(CMAKE_INSTALL_PREFIX "/usr/local/" CACHE PATH "Installation Directory" FORCE)

install(TARGETS ${LIBNAME}
    LIBRARY DESTINATION lib/chung
    RUNTIME DESTINATION lib/chung
)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/ DESTINATION include)