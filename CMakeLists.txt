cmake_minimum_required(VERSION 3.3)
project(polygon_clipping)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -ansi -pedantic -O0 -framework GLUT -framework OpenGL")

set(SOURCE_FILES main.cpp algorithms.cpp a_edge.cpp a_edge.h)
add_executable(polygon_clipping ${SOURCE_FILES})