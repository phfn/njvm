cmake_minimum_required(VERSION 3.10)
add_executable(njvm njvm.c)
project(njvm)

set(SOURCE_FILES njvm.c)
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall -std=c99 -pedantic")