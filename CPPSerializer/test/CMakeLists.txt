cmake_minimum_required(VERSION 3.0.0)

add_executable(binbuff_test 
    Game.h
    Game.cpp
    Player.h
    Player.cpp
    Test.cpp)

target_link_libraries(binbuff_test binbuff)