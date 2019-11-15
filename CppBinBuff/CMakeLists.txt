cmake_minimum_required(VERSION 3.0.0)

project(BinBuff)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)

add_subdirectory(test)

set(INCLUDE
        include/binbuff_traits.h
    include/buffer.h
    include/serializable.h include/binbuff.h)

set(SOURCES src/buffer.cpp)

add_library(binbuff STATIC ${INCLUDE} ${SOURCES})

target_include_directories(binbuff PUBLIC
    ${CMAKE_SOURCE_DIR}/include)