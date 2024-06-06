cmake_minimum_required(VERSION 3.16)
project(Final_Project)

set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE SOURCES "src/*")
add_executable(Final_Project ${SOURCES})
