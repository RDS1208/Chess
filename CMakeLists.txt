cmake_minimum_required(VERSION 4.3)
project(Chess)

set(CMAKE_CXX_STANDARD 20)

add_executable(Chess
        src/main.cpp
        src/Board.cpp
        src/Console.cpp
        src/Position.cpp
        include/Board.h
        include/Console.h
        include/Move.h
        include/Position.h
        src/Rules.cpp
        include/Rules.h
)
target_include_directories(Chess PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")

include(CTest)

if(BUILD_TESTING)
    add_executable(ChessMoveTests tests/MoveTests.cpp src/Board.cpp src/Console.cpp)
    target_include_directories(ChessMoveTests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
    add_test(NAME MoveParsing COMMAND ChessMoveTests)

    add_executable(ChessKnightTests tests/KnightMovesTests.cpp src/Board.cpp src/Position.cpp src/Rules.cpp)
    target_include_directories(ChessKnightTests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
    add_test(NAME KnightMoves COMMAND ChessKnightTests)

    add_executable(ChessPseudoLegalTests tests/PseudoLegalMovesTests.cpp src/Board.cpp src/Position.cpp src/Rules.cpp)
    target_include_directories(ChessPseudoLegalTests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
    add_test(NAME PseudoLegalMoves COMMAND ChessPseudoLegalTests)
endif()
