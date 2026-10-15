//
// Created by rds on 10/15/26.
//


#include "Console.h"

char pieceSymbol(const Piece& piece) {

    if (piece.type == PieceType::None)
        return '.';

    switch (piece.type) {
        case PieceType::Pawn:
            return piece.color == Color::White ? 'P' : 'p';
        case PieceType::Knight:
            return piece.color == Color::White ? 'N' : 'n';
        case PieceType::Bishop:
            return piece.color == Color::White ? 'B' : 'b';
        case PieceType::Rook:
            return piece.color == Color::White ? 'R' : 'r';
        case PieceType::Queen:
            return piece.color == Color::White ? 'Q' : 'q';
        case PieceType::King:
            return piece.color == Color::White ? 'K' : 'k';
        default:
            return '?'; // Should never reach here
    }
}

void printBoard(const Board& board, std::ostream& output) {
    for (int rank = 7; rank >= 0; --rank) {
        output << rank + 1 << " "; // Print rank number
        for (int file = 0; file < 8; ++file) {
            const Piece& piece = board.at(file, rank);
            output << pieceSymbol(piece) << " ";
        }
        output << "\n";
    }
    output << "  a b c d e f g h\n"; // Print file letters
}