#include "Rules.h"

#include <array>
#include <initializer_list>
#include <iostream>
#include <set>
#include <stdexcept>

bool expectKnightMoves(const Position& position,
                       std::initializer_list<std::array<int, 4>> expected,
                       const char* name) {
    const auto moves = rules::generatePseudoLegalMoves(position);
    std::set<std::array<int, 4>> actual;
    std::size_t count = 0;
    for (const auto& move : moves) {
        if (position.board.at(move.from.file, move.from.rank).type != PieceType::Knight) {
            continue;
        }
        if (move.promotion != PieceType::None) {
            std::cerr << name << ": knight move has a promotion\n";
            return false;
        }
        ++count;
        actual.insert({move.from.file, move.from.rank, move.to.file, move.to.rank});
    }

    if (actual != std::set<std::array<int, 4>>(expected) || count != actual.size()) {
        std::cerr << name << ": unexpected or duplicate knight moves\n";
        return false;
    }
    return true;
}

int main() {
    int failures = 0;
    auto check = [&failures](bool result) {
        if (!result) {
            ++failures;
        }
    };

    Position empty;
    check(expectKnightMoves(empty, {}, "empty board"));

    auto initial = createStartingPosition();
    check(expectKnightMoves(initial, {
        {1, 0, 0, 2}, {1, 0, 2, 2}, {6, 0, 5, 2}, {6, 0, 7, 2}
    }, "White's opening moves and jumping over pawns"));
    initial.sideToMove = Color::Black;
    check(expectKnightMoves(initial, {
        {1, 7, 0, 5}, {1, 7, 2, 5}, {6, 7, 5, 5}, {6, 7, 7, 5}
    }, "Black's opening moves and jumping over pawns"));

    Position center;
    center.board.setPiece(3, 3, Piece{PieceType::Knight, Color::White});
    check(expectKnightMoves(center, {
        {3, 3, 4, 5}, {3, 3, 5, 4}, {3, 3, 5, 2}, {3, 3, 4, 1},
        {3, 3, 2, 1}, {3, 3, 1, 2}, {3, 3, 1, 4}, {3, 3, 2, 5}
    }, "all eight knight offsets"));

    Position corners;
    corners.board.setPiece(0, 0, Piece{PieceType::Knight, Color::White});
    corners.board.setPiece(7, 7, Piece{PieceType::Knight, Color::White});
    check(expectKnightMoves(corners, {
        {0, 0, 1, 2}, {0, 0, 2, 1}, {7, 7, 5, 6}, {7, 7, 6, 5}
    }, "corner moves do not leave or wrap around the board"));

    for (const auto color : {Color::White, Color::Black}) {
        const auto opponent = color == Color::White ? Color::Black : Color::White;
        Position captures;
        captures.sideToMove = color;
        captures.board.setPiece(3, 3, Piece{PieceType::Knight, color});
        captures.board.setPiece(4, 5, Piece{PieceType::Pawn, color});
        captures.board.setPiece(5, 4, Piece{PieceType::Pawn, opponent});
        captures.board.setPiece(5, 2, Piece{PieceType::King, opponent});
        captures.board.setPiece(4, 1, Piece{PieceType::King, color});
        captures.board.setPiece(7, 7, Piece{PieceType::Knight, opponent});
        check(expectKnightMoves(captures, {
            {3, 3, 5, 4}, {3, 3, 2, 1}, {3, 3, 1, 2},
            {3, 3, 1, 4}, {3, 3, 2, 5}
        }, "captures, friendly blockers, opposing knights, and king exclusion"));

        if (captures.board.at(5, 4).type != PieceType::Pawn ||
            captures.board.at(5, 4).color != opponent ||
            captures.board.at(3, 3).type != PieceType::Knight) {
            std::cerr << "Generating moves changed the board\n";
            ++failures;
        }
    }

    Board board;
    board.setPiece(0, 0, Piece{PieceType::Rook, Color::Black});
    if (board.at(0, 0).type != PieceType::Rook ||
        board.at(0, 0).color != Color::Black) {
        std::cerr << "Setting a piece failed\n";
        ++failures;
    }
    for (const auto coords : std::array<std::array<int, 2>, 6>{{
             {-1, 0}, {8, 0}, {0, -1}, {0, 8}, {-1, 1}, {8, -1}}}) {
        try {
            board.setPiece(coords[0], coords[1], Piece{});
            std::cerr << "Setting a piece accepted invalid coordinates\n";
            ++failures;
        } catch (const std::out_of_range&) {
        }
    }
    if (board.at(0, 0).type != PieceType::Rook) {
        std::cerr << "Invalid coordinates changed the board\n";
        ++failures;
    }

    if (failures != 0) {
        return 1;
    }
    std::cout << "All knight move and board setup checks passed.\n";
}
