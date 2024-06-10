cmake_minimum_required (VERSION 2.8)
#set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")	//this style is for older version of cmake
set (CMAKE_CXX_STANDARD 11)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")


project (parallel-mbe)

add_library(my_lib undirectedgraph.cpp undirectedgraph.h utils.h global.h Lmbc.cpp Lmbc.h ParLmbc.cpp ParLmbc.h)


