#include "../include/Rules.h"

#include <array>
#include <iostream>
#include <set>
#include <utility>

using Squares = std::set<std::pair<int, int>>;

bool expectAttacks(const Position& position, Color color, const Squares& expected,
                   const char* name) {
    bool passed = true;
    for (int rank = 0; rank < 8; ++rank) {
        for (int file = 0; file < 8; ++file) {
            const bool actual = rules::isSquareAttacked(position, Square{file, rank}, color);
            if (actual != expected.contains({file, rank})) {
                std::cerr << name << ": incorrect attack at (" << file << ", " << rank << ")\n";
                passed = false;
            }
        }
    }
    return passed;
}

int main() {
    int failures = 0;
    auto check = [&failures](bool condition, const char* name) {
        if (!condition) {
            std::cerr << name << '\n';
            ++failures;
        }
    };

    struct PatternCase {
        PieceType type;
        Square source;
        Squares targets;
    };
    const std::array patterns{
        PatternCase{PieceType::Knight, {3, 3}, {
            {4, 5}, {5, 4}, {5, 2}, {4, 1}, {2, 1}, {1, 2}, {1, 4}, {2, 5}
        }},
        PatternCase{PieceType::King, {3, 3}, {
            {2, 2}, {2, 3}, {2, 4}, {3, 2}, {3, 4}, {4, 2}, {4, 3}, {4, 4}
        }},
        PatternCase{PieceType::Rook, {3, 3}, {
            {0, 3}, {1, 3}, {2, 3}, {4, 3}, {5, 3}, {6, 3}, {7, 3},
            {3, 0}, {3, 1}, {3, 2}, {3, 4}, {3, 5}, {3, 6}, {3, 7}
        }},
        PatternCase{PieceType::Bishop, {3, 3}, {
            {4, 4}, {5, 5}, {6, 6}, {7, 7}, {2, 4}, {1, 5}, {0, 6},
            {4, 2}, {5, 1}, {6, 0}, {2, 2}, {1, 1}, {0, 0}
        }},
        PatternCase{PieceType::Queen, {3, 3}, {
            {0, 3}, {1, 3}, {2, 3}, {4, 3}, {5, 3}, {6, 3}, {7, 3},
            {3, 0}, {3, 1}, {3, 2}, {3, 4}, {3, 5}, {3, 6}, {3, 7},
            {4, 4}, {5, 5}, {6, 6}, {7, 7}, {2, 4}, {1, 5}, {0, 6},
            {4, 2}, {5, 1}, {6, 0}, {2, 2}, {1, 1}, {0, 0}
        }},
        PatternCase{PieceType::Knight, {0, 0}, {{1, 2}, {2, 1}}},
        PatternCase{PieceType::King, {7, 7}, {{6, 6}, {6, 7}, {7, 6}}}
    };

    for (const auto color : {Color::White, Color::Black}) {
        const auto opponent = color == Color::White ? Color::Black : Color::White;
        for (const auto& test : patterns) {
            Position position;
            position.sideToMove = opponent;
            position.board.setPiece(test.source.file, test.source.rank, Piece{test.type, color});
            check(expectAttacks(position, color, test.targets, "piece attack pattern"),
                  "Attack pattern must work independently of side to move");
            check(expectAttacks(position, opponent, {}, "opposite color"),
                  "Ignore pieces of the other attacking color");
        }

        Position pawn;
        pawn.sideToMove = opponent;
        pawn.board.setPiece(3, 3, Piece{PieceType::Pawn, color});
        const Squares pawnTargets = color == Color::White
            ? Squares{{2, 4}, {4, 4}} : Squares{{2, 2}, {4, 2}};
        check(expectAttacks(pawn, color, pawnTargets, "pawn attacks"),
              "Pawns attack empty diagonals, never forward or backward");
        pawn.board.clear();
        pawn.board.setPiece(0, 3, Piece{PieceType::Pawn, color});
        pawn.board.setPiece(7, 3, Piece{PieceType::Pawn, color});
        const Squares edgeTargets = color == Color::White
            ? Squares{{1, 4}, {6, 4}} : Squares{{1, 2}, {6, 2}};
        check(expectAttacks(pawn, color, edgeTargets, "edge pawn attacks"),
              "Pawns on edge files do not wrap around");

        for (const auto blockerColor : {color, opponent}) {
            for (const auto slider : {PieceType::Rook, PieceType::Bishop, PieceType::Queen}) {
                Position blocked;
                blocked.board.setPiece(3, 3, Piece{slider, color});
                const Square blocker = slider == PieceType::Bishop ? Square{5, 5} : Square{3, 5};
                const Square beyond = slider == PieceType::Bishop ? Square{6, 6} : Square{3, 6};
                blocked.board.setPiece(blocker.file, blocker.rank, Piece{PieceType::Knight, blockerColor});
                check(rules::isSquareAttacked(blocked, blocker, color),
                      "First occupied square is attacked even when occupied by a friendly piece");
                check(!rules::isSquareAttacked(blocked, beyond, color),
                      "Friendly and enemy pieces block sliding attacks beyond their square");
            }
        }
    }

    Position empty;
    check(expectAttacks(empty, Color::White, {}, "empty board") &&
          expectAttacks(empty, Color::Black, {}, "empty board"),
          "Empty board has no attacks");

    Position pinned;
    pinned.board.setPiece(4, 0, Piece{PieceType::King, Color::White});
    pinned.board.setPiece(4, 1, Piece{PieceType::Knight, Color::White});
    pinned.board.setPiece(4, 7, Piece{PieceType::Rook, Color::Black});
    check(rules::isSquareAttacked(pinned, Square{2, 2}, Color::White),
          "A knight pinned to its king still attacks c3");

    Position occupied;
    occupied.board.setPiece(4, 3, Piece{PieceType::Knight, Color::White});
    occupied.board.setPiece(5, 4, Piece{PieceType::Pawn, Color::White});
    occupied.board.setPiece(5, 5, Piece{PieceType::King, Color::Black});
    occupied.board.setPiece(3, 5, Piece{PieceType::Pawn, Color::White});
    check(rules::isSquareAttacked(occupied, Square{5, 5}, Color::White),
          "Knight jumps over pieces and attacks an enemy king");
    check(rules::isSquareAttacked(occupied, Square{3, 5}, Color::White),
          "Knight defends a friendly occupied target");
    check(rules::isSquareAttacked(occupied, Square{4, 4}, Color::Black),
          "King attacks adjacent squares");

    Position kings;
    kings.board.setPiece(4, 3, Piece{PieceType::King, Color::White});
    kings.board.setPiece(4, 5, Piece{PieceType::King, Color::Black});
    check(rules::isSquareAttacked(kings, Square{4, 4}, Color::White) &&
          rules::isSquareAttacked(kings, Square{4, 4}, Color::Black),
          "Both kings attack the intervening square even though neither can legally enter it");

    if (failures != 0) {
        return 1;
    }
    std::cout << "All square attack checks passed.\n";
}
