cmake_minimum_required(VERSION 3.20.0)
project(tester VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)




add_executable(${PROJECT_NAME})

# set(MAIN main.cpp)
# set(MAIN main2.cpp)
# set(MAIN main3.cpp)
# set(MAIN main4.cpp)
# set(MAIN main5.cpp)
set(MAIN main6.cpp)
# set(MAIN demo.cpp)
# set(MAIN demo_inte.cpp)
target_sources(${PROJECT_NAME} PRIVATE ${MAIN})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)


find_package(Catch2 3 REQUIRED)
find_package(GTest REQUIRED)
find_package(Threads REQUIRED)
find_package(SQLite3 REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE Catch2::Catch2WithMain)
target_link_libraries(${PROJECT_NAME} PUBLIC GTest::gmock GTest::gtest GTest::gtest_main Threads::Threads)
target_link_libraries(${PROJECT_NAME} PRIVATE SQLite::SQLite3)