#include "../include/Rules.h"

#include <iostream>
#include <stdexcept>

int main() {
    int failures = 0;
    auto check = [&failures](bool condition, const char* name) {
        if (!condition) {
            std::cerr << name << '\n';
            ++failures;
        }
    };

    for (const auto color : {Color::White, Color::Black}) {
        const int fromRank = color == Color::White ? 6 : 1;
        const int toRank = color == Color::White ? 7 : 0;
        const auto opponent = color == Color::White ? Color::Black : Color::White;
        Position position;
        position.sideToMove = color;
        position.board.setPiece(0, 0, Piece{PieceType::King, Color::White});
        position.board.setPiece(7, 7, Piece{PieceType::King, Color::Black});
        position.board.setPiece(4, fromRank, Piece{PieceType::Pawn, color});
        position.board.setPiece(3, toRank, Piece{PieceType::Rook, opponent});

        const auto moves = rules::generateLegalMoves(position);
        int pawnMoveCount = 0;
        for (const Move& move : moves) {
            if (move.from.file == 4 && move.from.rank == fromRank) {
                ++pawnMoveCount;
                check(move.promotion == PieceType::Queen || move.promotion == PieceType::Rook ||
                      move.promotion == PieceType::Bishop || move.promotion == PieceType::Knight,
                      "Every final-rank pawn move must choose a valid promotion piece");
            }
        }
        check(pawnMoveCount == 8, "One quiet destination and one capture each offer four choices");

        for (const auto choice : {PieceType::Queen, PieceType::Rook,
                                  PieceType::Bishop, PieceType::Knight}) {
            for (const int toFile : {3, 4}) {
                int matches = 0;
                for (const Move& move : moves) {
                    if (move.from.file != 4 || move.from.rank != fromRank ||
                        move.to.file != toFile || move.to.rank != toRank ||
                        move.promotion != choice) {
                        continue;
                    }
                    ++matches;
                    Position result = position;
                    applyMove(result, move);
                    check(result.board.at(toFile, toRank).type == choice &&
                          result.board.at(toFile, toRank).color == color,
                          "Selected piece replaces pawn and keeps its color");
                    check(result.board.at(4, fromRank).type == PieceType::None,
                          "Promotion clears pawn's source square");
                    check(result.sideToMove == opponent && result.halfmoveClock == 0,
                          "Promotion switches turn and resets halfmove clock");
                }
                check(matches == 1, "Quiet moves and captures each offer every promotion choice once");
            }
        }

        for (const auto invalidChoice : {PieceType::None, PieceType::Pawn, PieceType::King}) {
            for (const int toFile : {3, 4}) {
                Position result = position;
                bool rejected = false;
                try {
                    applyMove(result, Move{{4, fromRank}, {toFile, toRank}, invalidChoice});
                } catch (const std::invalid_argument&) {
                    rejected = true;
                }
                check(rejected, "Missing promotion choice, pawn, and king are rejected");
                check(result.board.at(4, fromRank).type == PieceType::Pawn &&
                      result.board.at(toFile, toRank).type == position.board.at(toFile, toRank).type &&
                      result.sideToMove == color,
                      "Rejected promotion leaves source, destination, and turn unchanged");
            }
        }
    }

    for (const Move& move : rules::generateLegalMoves(createStartingPosition())) {
        check(move.promotion == PieceType::None, "Ordinary opening moves do not offer promotion");
    }

    for (const Move move : {Move{{4, 1}, {4, 3}, PieceType::Queen},
                            Move{{1, 0}, {2, 2}, PieceType::Queen}}) {
        auto position = createStartingPosition();
        bool rejected = false;
        try {
            applyMove(position, move);
        } catch (const std::invalid_argument&) {
            rejected = true;
        }
        check(rejected, "Ordinary pawn and knight moves cannot have promotion choices");
    }

    if (failures != 0) {
        return 1;
    }
    std::cout << "All promotion checks passed.\n";
}
