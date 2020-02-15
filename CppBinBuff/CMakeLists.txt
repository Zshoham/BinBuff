cmake_minimum_required(VERSION 3.0.0)

project(BinBuff C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

enable_testing()

add_subdirectory(googletest)
add_subdirectory(test)

set(INCLUDE
    include/binbuff_traits.h
    include/buffer.h
    include/serializable.h 
    include/binbuff.h
)

set(SOURCES src/buffer.cpp)

add_library(binbuff STATIC ${INCLUDE} ${SOURCES})

target_include_directories(binbuff PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

install(
    TARGETS binbuff binbuff_test
    COMPONENT binbuff
    CONFIGURATIONS Debug
    ARCHIVE DESTINATION ${CMAKE_SOURCE_DIR}/bin/Debug/${CMAKE_SYSTEM_NAME}
    LIBRARY DESTINATION ${CMAKE_SOURCE_DIR}/bin/Debug/${CMAKE_SYSTEM_NAME}
    RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/bin/Debug/${CMAKE_SYSTEM_NAME}
)

install(
    TARGETS binbuff binbuff_test
    COMPONENT binbuff
    CONFIGURATIONS Release
    ARCHIVE DESTINATION ${CMAKE_SOURCE_DIR}/bin/Release/${CMAKE_SYSTEM_NAME}
    LIBRARY DESTINATION ${CMAKE_SOURCE_DIR}/bin/Release/${CMAKE_SYSTEM_NAME}
    RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/bin/Release/${CMAKE_SYSTEM_NAME}
)

install(
    FILES ${INCLUDE} DESTINATION ${CMAKE_SOURCE_DIR}/bin/include 
    COMPONENT binbuff
)
