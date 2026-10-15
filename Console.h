//
// Created by rds on 10/15/26.
//

#ifndef CHESS_CONSOLE_H
#define CHESS_CONSOLE_H
#include <ostream>
#include "Board.h"
#include <iosfwd>

char pieceSymbol(const Piece& piece);
void printBoard(const Board& board, std::ostream& output);


#endif //CHESS_CONSOLE_H
