cmake_minimum_required(VERSION 3.10)

project(FormalPrac)
set(CMAKE_CXX_FLAGS "-std=c++17")
add_executable(FormalPrac regex.cpp regex_automata.cpp)