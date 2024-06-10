cmake_minimum_required(VERSION 3.3.2)
project(MBE)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-O3")
set(SOURCE_FILES
        BicliqueFinder.cpp
        BicliqueFinder.h
        BiGraph.cpp
        BiGraph.h
        Utility.cpp
        Utility.h
        BaselineFinder.cpp
        BaselineFinder.h
        AdaFinder.cpp
        AdaFinder.h
        ParAdaFinder.cpp
        ParAdaFinder.h
        main.cpp)

set(TBB_ROOT "oneTBB")
include_directories(${TBB_ROOT}/include)
link_directories(${TBB_ROOT}/lib)
add_executable(MBE_ALL ${SOURCE_FILES})
target_link_libraries(MBE_ALL tbb)

set(NUMBERS 0 4 8 16 24 32 40 48 56 64 80 96 112 128 144 160 176 192 208 224 240 256)

foreach(num ${NUMBERS})
add_executable(MBE_${num} ${SOURCE_FILES})
target_compile_definitions(MBE_${num} PRIVATE -DNN=${num})
target_link_libraries(MBE_${num} tbb)
endforeach()
