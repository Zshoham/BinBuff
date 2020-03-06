cmake_minimum_required(VERSION 3.0.0)

add_executable(binbuff_test 
    Game.h
    Game.cpp
    Player.h
    Player.cpp
    test.cpp)

target_link_libraries(binbuff_test 
	PUBLIC 
    binbuff
    gtest_main
)

add_test(
    NAME binbuff_test
    COMMAND ./binbuff_test
)

install(
    TARGETS binbuff_test
    COMPONENT binbuff
    CONFIGURATIONS Release
    RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/bin/Release/${CMAKE_SYSTEM_NAME}
)

install(
    TARGETS binbuff_test
    COMPONENT binbuff
    CONFIGURATIONS Debug
    RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/bin/Debug/${CMAKE_SYSTEM_NAME}
)
