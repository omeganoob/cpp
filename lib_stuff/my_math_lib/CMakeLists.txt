set(LIBNAME my_math_lib)
set(SRC_MATHLIB my_math_lib.cpp)

add_library(${LIBNAME})

target_sources(${LIBNAME} PRIVATE ${SRC_MATHLIB})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

target_include_directories (${PROJECT_NAME} PUBLIC 
        "${CMAKE_CURRENT_SOURCE_DIR}"
        "${CMAKE_CURRENT_SOURCE_DIR}/include")