#pragma once

#include <array>

enum class PieceType {
    None,
    Pawn,
    Knight,
    Bishop,
    Rook,
    Queen,
    King
};

enum class Color {
    White,
    Black
};

struct Piece {
    PieceType type = PieceType::None;
    Color color = Color::White;
};

class Board {
public:
    const Piece& at(int file, int rank) const;
    void setPiece(int file, int rank, Piece piece);
    void clear();
    void resetStartingPosition();

private:
    std::array<Piece, 64> squares{};
};
