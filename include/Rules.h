//
// Created by rds on 10/15/26.
//

#ifndef CHESS_RULES_H
#define CHESS_RULES_H
#include "Move.h"
#include "Position.h"
#include <vector>

namespace rules {
    std::vector<Move> generatePseudoLegalMoves(const Position& position);
    std::vector<Move> generateLegalMoves(const Position& position);
    bool isInCheck(const Position& position, Color color);
}

#endif //CHESS_RULES_H
