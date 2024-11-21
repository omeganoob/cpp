cmake_minimum_required(VERSION 3.10.0)
project(pattern VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 23)
add_executable(${PROJECT_NAME})

# set(MAIN main.cpp)
# set(MAIN main2.cpp)
# set(MAIN main3.cpp)
set(MAIN main4.cpp)
set(SINGLETON src/creational/singleton.cpp)
set(OBSERVER src/behavioral/observer_implement.cpp)
set(PROXY src/structural/proxy.cpp)

target_sources(${PROJECT_NAME} PRIVATE 
                                ${MAIN} 
                                ${SINGLETON}
                                ${OBSERVER}
                                ${PROXY} )

include_directories(${PROJECT_SOURCE_DIR}/include)
