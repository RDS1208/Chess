//
// Created by rds on 10/15/26.
//

#ifndef CHESS_CONSOLE_H
#define CHESS_CONSOLE_H
#include <ostream>
#include "Board.h"
#include "Move.h"
#include <iosfwd>
#include <optional>
#include <string>

char pieceSymbol(const Piece& piece);
void printBoard(const Board& board, std::ostream& output);
std::optional<Move> parseMove(const std::string& text);


#endif //CHESS_CONSOLE_H
