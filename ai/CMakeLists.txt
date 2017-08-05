cmake_minimum_required(VERSION 2.8)
add_definitions("-Wall -std=c++11")

add_subdirectory(../core ../core)
add_executable(nullai nullai.cpp)
target_link_libraries(nullai driver)
