cmake_minimum_required(VERSION 3.0.0)

add_executable(binbuff_test 
    Game.h
    Game.cpp
    Player.h
    Player.cpp
    Test.cpp
    test.cpp)

target_link_libraries(binbuff_test PUBLIC 
    binbuff
    gtest_main
)

add_test(
    NAME binbuff_test
    COMMAND binbuff_test
)