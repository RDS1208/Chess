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
        src/Game.cpp
        include/Game.h
)
target_include_directories(Chess PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")

# Graphical interface; the terminal target can also be built without SFML
option(CHESS_BUILD_GUI "Build the SFML graphical interface" ON)
if(CHESS_BUILD_GUI)
    find_package(SFML 3 COMPONENTS Graphics QUIET)
    if(NOT SFML_FOUND)
        include(FetchContent)
        set(SFML_BUILD_AUDIO OFF CACHE BOOL "" FORCE)
        set(SFML_BUILD_NETWORK OFF CACHE BOOL "" FORCE)
        set(SFML_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
        set(SFML_BUILD_TEST_SUITE OFF CACHE BOOL "" FORCE)
        FetchContent_Declare(SFML
                URL https://github.com/SFML/SFML/archive/refs/tags/3.0.2.tar.gz
                URL_HASH SHA256=0034e05f95509e5d3fb81b1625713e06da7b068f210288ce3fd67106f8f46995
                DOWNLOAD_EXTRACT_TIMESTAMP TRUE
                EXCLUDE_FROM_ALL
                SYSTEM)
        FetchContent_MakeAvailable(SFML)
    endif()
    add_executable(ChessGUI src/gui_main.cpp src/Gui.cpp include/Gui.h
            src/Game.cpp src/Board.cpp src/Console.cpp src/Position.cpp src/Rules.cpp)
    target_include_directories(ChessGUI PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
    target_link_libraries(ChessGUI PRIVATE SFML::Graphics)
    target_compile_definitions(ChessGUI PRIVATE
            CHESS_FONT_PATH="$<TARGET_FILE_DIR:ChessGUI>/assets/fonts/DejaVuSans.ttf"
            CHESS_PIECES_PATH="$<TARGET_FILE_DIR:ChessGUI>/assets/pieces/neo")
    add_custom_command(TARGET ChessGUI POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/assets" "$<TARGET_FILE_DIR:ChessGUI>/assets")
endif()

include(CTest)

if(BUILD_TESTING AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests")
    add_executable(ChessMoveTests tests/MoveTests.cpp src/Board.cpp src/Console.cpp)
    target_include_directories(ChessMoveTests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
    add_test(NAME MoveParsing COMMAND ChessMoveTests)

    add_executable(ChessKnightTests tests/KnightMovesTests.cpp src/Board.cpp src/Position.cpp src/Rules.cpp)
    target_include_directories(ChessKnightTests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
    add_test(NAME KnightMoves COMMAND ChessKnightTests)

    add_executable(ChessPseudoLegalTests tests/PseudoLegalMovesTests.cpp src/Board.cpp src/Position.cpp src/Rules.cpp)
    target_include_directories(ChessPseudoLegalTests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
    add_test(NAME PseudoLegalMoves COMMAND ChessPseudoLegalTests)

    add_executable(ChessPositionTests tests/PositionTests.cpp src/Board.cpp src/Position.cpp)
    target_include_directories(ChessPositionTests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
    add_test(NAME MoveApplication COMMAND ChessPositionTests)

    add_executable(ChessAttackTests tests/AttackTests.cpp src/Board.cpp src/Position.cpp src/Rules.cpp)
    target_include_directories(ChessAttackTests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
    add_test(NAME SquareAttacks COMMAND ChessAttackTests)

    add_executable(ChessCheckTests tests/CheckTests.cpp src/Board.cpp src/Position.cpp src/Rules.cpp)
    target_include_directories(ChessCheckTests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
    add_test(NAME CheckDetection COMMAND ChessCheckTests)

    add_executable(ChessPromotionTests tests/PromotionTests.cpp src/Board.cpp src/Position.cpp src/Rules.cpp)
    target_include_directories(ChessPromotionTests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
    add_test(NAME Promotion COMMAND ChessPromotionTests)

    add_executable(ChessEnPassantTests tests/EnPassantTests.cpp src/Board.cpp src/Position.cpp src/Rules.cpp)
    target_include_directories(ChessEnPassantTests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
    add_test(NAME EnPassant COMMAND ChessEnPassantTests)

    add_executable(ChessCastlingTests tests/CastlingTests.cpp src/Board.cpp src/Position.cpp src/Rules.cpp)
    target_include_directories(ChessCastlingTests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
    add_test(NAME Castling COMMAND ChessCastlingTests)

    add_executable(ChessGameTests tests/GameTests.cpp src/Game.cpp src/Board.cpp src/Console.cpp src/Position.cpp src/Rules.cpp)
    target_include_directories(ChessGameTests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
    add_test(NAME GameMoves COMMAND ChessGameTests)

    add_executable(ChessDrawTests tests/DrawTests.cpp src/Game.cpp src/Board.cpp src/Console.cpp src/Position.cpp src/Rules.cpp)
    target_include_directories(ChessDrawTests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
    add_test(NAME DrawRules COMMAND ChessDrawTests)

    if(TARGET ChessGUI)
        add_executable(ChessGuiTests tests/GuiTests.cpp src/Gui.cpp
                src/Game.cpp src/Board.cpp src/Console.cpp src/Position.cpp src/Rules.cpp)
        target_include_directories(ChessGuiTests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
        target_link_libraries(ChessGuiTests PRIVATE SFML::Graphics)
        add_test(NAME GraphicalInput COMMAND ChessGuiTests)
    endif()
endif()
