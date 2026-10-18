//
// Created by rds on 10/15/26.
//

#ifndef CHESS_RULES_H
#define CHESS_RULES_H
#include "Move.h"
#include "Position.h"
#include <vector>

namespace rules {
    bool canPromote(const Piece& piece, Square destination);
    std::vector<Move> generatePseudoLegalMoves(const Position& position);
    std::vector<Move> generateLegalMoves(const Position& position);
    // Queries a board square independently of side to move and king-safety restrictions.
    bool isSquareAttacked(const Position& position, Square square, Color attackingColor);
    // Throws std::logic_error if the requested color's king is missing.
    bool isInCheck(const Position& position, Color color);
    bool isSamePosition(const Position& a, const Position& b);
    // History contains previous positions; current is counted separately
    int countRepetitions(const std::vector<Position>& history, const Position& current);
    bool threeFoldRepetition(const std::vector<Position>& history, const Position& current);
    bool fiveFoldRepetition(const std::vector<Position>& history, const Position& current);
    bool isDeadPositionInsufficientMaterial(const Position& position);
}

#endif //CHESS_RULES_H
