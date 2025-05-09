cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)


file(GLOB CPP_SOURCE 
    *.cpp
)

add_executable(PBRman ${CPP_SOURCE})