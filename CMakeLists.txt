cmake_minimum_required (VERSION 2.8)

set (CMAKE_CXX_FLAGS "-Wall -std=c++11" )
set (CMAKE_CXX_FLAGS_DEBUG "-g3 -O0" )
set (CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG" )
add_executable ( MIPS MIPS.cpp)
