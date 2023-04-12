cmake_minimum_required(VERSION 3.14)

set(This "SML_Parser")
project(${This})

set(CMAKE_CXX_STANDARD 14)



add_subdirectory(src)
add_subdirectory(test)