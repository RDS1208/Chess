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


// Applies a generated move, including promotion, en passant, and castling, without checking legality.
// Throws std::invalid_argument for a missing/invalid promotion or a choice on another move.
void applyMove(Position& position, const Move& move);


Position createStartingPosition();

#endif //CHESS_POSITION_H
