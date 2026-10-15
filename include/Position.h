//
// Created by rds on 10/15/26.
//

#ifndef CHESS_POSITION_H
#define CHESS_POSITION_H

#include "Board.h"
#include "Move.h"

#include <optional>

struct Position {
    Board board;
    Color sideToMove = Color::White;

    bool whiteCanCastleKingside = false;
    bool whiteCanCastleQueenside = false;
    bool blackCanCastleKingside = false;
    bool blackCanCastleQueenside = false;

    std::optional<Square> enPassantTarget = std::nullopt;
    unsigned int halfmoveClock = 0;
    unsigned int fullmoveNumber = 1;
};

Position createStartingPosition();

#endif //CHESS_POSITION_H