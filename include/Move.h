//
// Created by rds on 10/15/26.
//

#ifndef CHESS_MOVE_H
#define CHESS_MOVE_H
#include "Board.h"

struct Square {
    int file = 0;
    int rank = 0;
};

struct Move {
    Square from;
    Square to;
    PieceType promotion = PieceType::None;
};

#endif //CHESS_MOVE_H
