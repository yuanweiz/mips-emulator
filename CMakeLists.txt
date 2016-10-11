cmake_minimum_required (VERSION 2.8)

find_package (GTest)
set (CMAKE_CXX_FLAGS "-Wall -std=c++11" )
set (CMAKE_CXX_FLAGS_DEBUG "-g3 -O0" )
set (CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG" )

add_library( MIPS MIPS.cpp)
add_executable ( main main.cpp)
target_link_libraries (main MIPS)
if (GTEST_FOUND)
add_executable ( tests tests.cpp)
target_link_libraries (tests MIPS gtest)
endif()
