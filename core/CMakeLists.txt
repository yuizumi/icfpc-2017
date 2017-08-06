cmake_minimum_required(VERSION 2.8)
add_definitions("-Wall -std=c++11 -O3")
add_library(driver STATIC driver.cpp)
