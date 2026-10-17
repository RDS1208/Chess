#include "../include/Rules.h"

#include <array>
#include <initializer_list>
#include <iostream>
#include <set>

bool expectTargets(const Position& position, Square from,
                   std::initializer_list<std::array<int, 2>> expected,
                   const char* name) {
    std::set<std::array<int, 2>> actual;
    std::size_t count = 0;
    for (const auto& move : rules::generatePseudoLegalMoves(position)) {
        if (move.from.file == from.file && move.from.rank == from.rank) {
            actual.insert({move.to.file, move.to.rank});
            ++count;
        }
    }
    if (actual != std::set<std::array<int, 2>>(expected) || count != actual.size()) {
        std::cerr << name << ": unexpected or duplicate destinations\n";
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

    auto initial = createStartingPosition();
    for (const auto color : {Color::White, Color::Black}) {
        initial.sideToMove = color;
        if (rules::generatePseudoLegalMoves(initial).size() != 20) {
            std::cerr << "Expected 20 opening moves\n";
            ++failures;
        }
    }

    struct PawnCase {
        Color color;
        int rank;
        int nextRank;
        int startRank;
        int firstStepRank;
        int secondStepRank;
    };
    const std::array pawnCases{
        PawnCase{Color::White, 3, 4, 1, 2, 3},
        PawnCase{Color::Black, 4, 3, 6, 5, 4}
    };

    for (const auto& test : pawnCases) {
        const auto opponent = test.color == Color::White ? Color::Black : Color::White;
        Position pawn;
        pawn.sideToMove = test.color;
        pawn.board.setPiece(4, test.rank, Piece{PieceType::Pawn, test.color});
        pawn.board.setPiece(4, test.nextRank, Piece{PieceType::Rook, opponent});
        pawn.board.setPiece(3, test.nextRank, Piece{PieceType::Pawn, opponent});
        pawn.board.setPiece(5, test.nextRank, Piece{PieceType::Pawn, opponent});
        check(expectTargets(pawn, Square{4, test.rank}, {
            {3, test.nextRank}, {5, test.nextRank}
        }, "both pawn captures remain available when forward is blocked"));

        pawn.board.setPiece(3, test.nextRank, Piece{PieceType::Pawn, test.color});
        pawn.board.setPiece(5, test.nextRank, Piece{PieceType::King, opponent});
        check(expectTargets(pawn, Square{4, test.rank}, {},
                            "pawns cannot capture friendly pieces or kings"));

        pawn.board.setPiece(4, test.nextRank, Piece{});
        pawn.board.setPiece(3, test.nextRank, Piece{});
        pawn.board.setPiece(5, test.nextRank, Piece{});
        check(expectTargets(pawn, Square{4, test.rank}, {{4, test.nextRank}},
                            "pawns cannot capture empty diagonal squares or double-step away from home"));

        Position edge;
        edge.sideToMove = test.color;
        edge.board.setPiece(0, test.rank, Piece{PieceType::Pawn, test.color});
        edge.board.setPiece(1, test.nextRank, Piece{PieceType::Pawn, opponent});
        edge.board.setPiece(7, test.rank, Piece{PieceType::Pawn, test.color});
        edge.board.setPiece(6, test.nextRank, Piece{PieceType::Pawn, opponent});
        check(expectTargets(edge, Square{0, test.rank}, {
            {0, test.nextRank}, {1, test.nextRank}
        }, "a-file pawn boundaries"));
        check(expectTargets(edge, Square{7, test.rank}, {
            {7, test.nextRank}, {6, test.nextRank}
        }, "h-file pawn boundaries"));

        Position start;
        start.sideToMove = test.color;
        start.board.setPiece(4, test.startRank, Piece{PieceType::Pawn, test.color});
        check(expectTargets(start, Square{4, test.startRank}, {
            {4, test.firstStepRank}, {4, test.secondStepRank}
        }, "single and double pawn advances"));
        start.board.setPiece(4, test.firstStepRank, Piece{PieceType::Pawn, opponent});
        check(expectTargets(start, Square{4, test.startRank}, {},
                            "blocked pawn cannot jump or capture forward"));
        start.board.setPiece(4, test.firstStepRank, Piece{});
        start.board.setPiece(4, test.secondStepRank, Piece{PieceType::Pawn, opponent});
        check(expectTargets(start, Square{4, test.startRank}, {{4, test.firstStepRank}},
                            "occupied second square prevents double advance"));

        Position king;
        king.sideToMove = test.color;
        king.board.setPiece(4, 3, Piece{PieceType::King, test.color});
        king.board.setPiece(5, 3, Piece{PieceType::Pawn, test.color});
        king.board.setPiece(4, 4, Piece{PieceType::King, opponent});
        king.board.setPiece(3, 3, Piece{PieceType::Pawn, opponent});
        check(expectTargets(king, Square{4, 3}, {
            {3, 3}, {4, 2}, {5, 4}, {5, 2}, {3, 4}, {3, 2}
        }, "king can capture enemy pieces but not friendly pieces or the enemy king"));
    }

    if (failures != 0) {
        return 1;
    }
    std::cout << "All ordinary pawn and king move checks passed.\n";
}
