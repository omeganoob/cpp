cmake_minimum_required(VERSION 3.20.0)
project(lib_test VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 23)

set(MAIN main.cpp)

add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME} PRIVATE ${MAIN})

set(MATH_LIB_PATH /mnt/e/fpt_fresher/cpp/lib_study/lib)
set(MY_MATH_LIB /mnt/e/fpt_fresher/cpp/lib_study/lib/bin/libmy_math_lib.a)

set(VEC_LIB_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../dll)
set(VEC_LIB ${VEC_LIB_PATH}/bin/libmy_dlib.so)

target_include_directories(${PROJECT_NAME} 
            PRIVATE 
            ${MATH_LIB_PATH}/include 
            ${VEC_LIB_PATH}/include)
            
target_link_libraries(${PROJECT_NAME} PRIVATE ${MY_MATH_LIB} ${VEC_LIB})
