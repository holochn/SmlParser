cmake_minimum_required(VERSION 3.14)

set(This "SmlParser")
project(${This})

set(CMAKE_CXX_STANDARD 17)



add_subdirectory(src)
add_subdirectory(test)