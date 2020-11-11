cmake_minimum_required(VERSION 3.10)
add_executable(njvm njvm.c)
project(njvm)
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall -std=c99 -pedantic")

#copy all progs and the Ninja asambler
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/progs
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/nja
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR})