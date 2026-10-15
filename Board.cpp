#include "Board.h"

#include <stdexcept>

const Piece& Board::at(int file, int rank) const {
    if (file < 0 || file >= 8 || rank < 0 || rank >= 8) {
        throw std::out_of_range("Board coordinates must be between 0 and 7");
    }

    return squares[rank * 8 + file];
}
void Board::clear() {
    for (auto& square : squares) {
        square = Piece();
    }
}

void Board::ResetStartingPosition() {
    clear();

    // Set up pawns
    for (int file = 0; file < 8; ++file) {
        squares[1 * 8 + file] = Piece(PieceType::Pawn, Color::White);
        squares[6 * 8 + file] = Piece(PieceType::Pawn, Color::Black);
    }

    // Set up rooks
    squares[0 * 8 + 0] = Piece(PieceType::Rook, Color::White);
    squares[0 * 8 + 7] = Piece(PieceType::Rook, Color::White);
    squares[7 * 8 + 0] = Piece(PieceType::Rook, Color::Black);
    squares[7 * 8 + 7] = Piece(PieceType::Rook, Color::Black);

    // Set up knights
    squares[0 * 8 + 1] = Piece(PieceType::Knight, Color::White);
    squares[0 * 8 + 6] = Piece(PieceType::Knight, Color::White);
    squares[7 * 8 + 1] = Piece(PieceType::Knight, Color::Black);
    squares[7 * 8 + 6] = Piece(PieceType::Knight, Color::Black);

    // Set up bishops
    squares[0 * 8 + 2] = Piece(PieceType::Bishop, Color::White);
    squares[0 * 8 + 5] = Piece(PieceType::Bishop, Color::White);
    squares[7 * 8 + 2] = Piece(PieceType::Bishop, Color::Black);
    squares[7 * 8 + 5] = Piece(PieceType::Bishop, Color::Black);

    // Set up queens
    squares[0 * 8 + 3] = Piece(PieceType::Queen, Color::White);
    squares[7 * 8 + 3] = Piece(PieceType::Queen, Color::Black);

    // Set up kings
    squares[0 * 8 + 4] = Piece(PieceType::King, Color::White);
    squares[7 * 8 + 4] = Piece(PieceType::King, Color::Black);
}
