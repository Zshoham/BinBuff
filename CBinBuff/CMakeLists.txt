cmake_minimum_required(VERSION 3.0.0)

project(binbuff C)

set(CMAKE_C_STANDARD 11)

add_library(binbuff
        binbuff.h
        binbuff.c)

add_executable(binbuff_test 
    binbuff.h
    binbuff.c
    test.c)