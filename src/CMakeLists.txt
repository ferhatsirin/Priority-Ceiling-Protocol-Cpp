cmake_minimum_required(VERSION 3.10)

project(Ceiling)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")

set(SOURCES main.cpp mutex.h mutex.cpp PortableThread.h PortableThread.cpp)

add_executable(Ceiling ${SOURCES})

target_link_libraries(Ceiling ${CMAKE_THREAD_LIBS_INIT})

