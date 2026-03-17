cmake_minimum_required(VERSION 3.31)
project(PEA_project_1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -static")

add_executable(PEA_project_1 main.cpp
        algorithms.cpp
        algorithms.h
        utils.cpp
        utils.h)

if(WIN32)
    target_link_libraries(PEA_project_1 psapi)
endif()