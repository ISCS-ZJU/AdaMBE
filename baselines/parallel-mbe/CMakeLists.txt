cmake_minimum_required (VERSION 3.3.2)
set (CMAKE_CXX_STANDARD 11)
cmake_policy(SET CMP0081 NEW)
#list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/..")
#set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

#find TBB libraries and header files
# find_library(TBB NAMES tbb PATHS /usr/lib/x86_64-linux-gnu/  REQUIRED)

set(TBB_ROOT "oneTBB")
include_directories(${TBB_ROOT}/include)
link_directories(${TBB_ROOT}/lib)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")


add_subdirectory(source)

project (parallel-mbe)

add_executable(mbe_test main.cpp)
target_link_libraries(mbe_test my_lib tbb)
