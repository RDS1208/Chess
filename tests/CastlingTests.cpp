#include "../include/Rules.h"

#include <iostream>

Position castlingPosition(Color color) {
    Position position = createStartingPosition();
    position.board.clear();
    position.sideToMove = color;
    const int homeRank = color == Color::White ? 0 : 7;
    const int otherRank = color == Color::White ? 7 : 0;
    const auto opponent = color == Color::White ? Color::Black : Color::White;
    position.board.setPiece(4, homeRank, Piece{PieceType::King, color});
    position.board.setPiece(0, homeRank, Piece{PieceType::Rook, color});
    position.board.setPiece(7, homeRank, Piece{PieceType::Rook, color});
    position.board.setPiece(1, otherRank, Piece{PieceType::King, opponent});
    return position;
}

bool hasCastle(const std::vector<Move>& moves, int rank, int destinationFile) {
    for (const Move& move : moves) {
        if (move.from.file == 4 && move.from.rank == rank &&
            move.to.file == destinationFile && move.to.rank == rank) {
            return true;
        }
    }
    return false;
}

int main() {
    int failures = 0;
    auto check = [&failures](bool condition, const char* name) {
        if (!condition) {
            std::cerr << name << '\n';
            ++failures;
        }
    };

    for (const auto color : {Color::White, Color::Black}) {
        const int homeRank = color == Color::White ? 0 : 7;
        const int attackRank = color == Color::White ? 3 : 4;
        const auto opponent = color == Color::White ? Color::Black : Color::White;

        for (const int destinationFile : {2, 6}) {
            const int rookFile = destinationFile == 6 ? 7 : 0;
            const int rookDestination = destinationFile == 6 ? 5 : 3;
            auto position = castlingPosition(color);
            check(hasCastle(rules::generatePseudoLegalMoves(position), homeRank, destinationFile),
                  "Clear path with rights permits a castling candidate");
            check(hasCastle(rules::generateLegalMoves(position), homeRank, destinationFile),
                  "Castling works for both sides and colors");

            position.halfmoveClock = 7;
            position.enPassantTarget = Square{3, 2};
            applyMove(position, Move{{4, homeRank}, {destinationFile, homeRank}});
            check(position.board.at(4, homeRank).type == PieceType::None &&
                  position.board.at(rookFile, homeRank).type == PieceType::None &&
                  position.board.at(destinationFile, homeRank).type == PieceType::King &&
                  position.board.at(destinationFile, homeRank).color == color &&
                  position.board.at(rookDestination, homeRank).type == PieceType::Rook &&
                  position.board.at(rookDestination, homeRank).color == color,
                  "Castling moves both king and rook and clears both source squares");
            check(position.sideToMove == opponent && position.halfmoveClock == 8 &&
                  position.fullmoveNumber == (color == Color::White ? 1u : 2u) &&
                  !position.enPassantTarget,
                  "Castling updates turn, clocks, and en passant target once");
            if (color == Color::White) {
                check(!position.whiteCanCastleKingside && !position.whiteCanCastleQueenside &&
                      position.blackCanCastleKingside && position.blackCanCastleQueenside,
                      "White castling removes only White's rights");
            } else {
                check(!position.blackCanCastleKingside && !position.blackCanCastleQueenside &&
                      position.whiteCanCastleKingside && position.whiteCanCastleQueenside,
                      "Black castling removes only Black's rights");
            }

            auto noRight = castlingPosition(color);
            if (color == Color::White) {
                if (destinationFile == 6) {
                    noRight.whiteCanCastleKingside = false;
                } else {
                    noRight.whiteCanCastleQueenside = false;
                }
            } else {
                if (destinationFile == 6) {
                    noRight.blackCanCastleKingside = false;
                } else {
                    noRight.blackCanCastleQueenside = false;
                }
            }
            check(!hasCastle(rules::generatePseudoLegalMoves(noRight), homeRank, destinationFile),
                  "Missing castling right prevents castling");
            check(hasCastle(rules::generateLegalMoves(noRight), homeRank, 8 - destinationFile),
                  "Removing one castling right preserves the other side");

            auto rook = castlingPosition(color);
            rook.board.setPiece(rookFile, homeRank, Piece{});
            check(!hasCastle(rules::generatePseudoLegalMoves(rook), homeRank, destinationFile),
                  "Missing rook prevents castling even with rights");
            rook.board.setPiece(rookFile, homeRank, Piece{PieceType::Rook, opponent});
            check(!hasCastle(rules::generatePseudoLegalMoves(rook), homeRank, destinationFile),
                  "Opposing rook cannot be used for castling");
            rook.board.setPiece(rookFile, homeRank, Piece{PieceType::Bishop, color});
            check(!hasCastle(rules::generatePseudoLegalMoves(rook), homeRank, destinationFile),
                  "Corner piece must be a rook");

            const int firstFile = destinationFile == 6 ? 5 : 1;
            const int lastFile = destinationFile == 6 ? 6 : 3;
            for (int file = firstFile; file <= lastFile; ++file) {
                auto blocked = castlingPosition(color);
                blocked.board.setPiece(file, homeRank, Piece{PieceType::Bishop, color});
                check(!hasCastle(rules::generatePseudoLegalMoves(blocked), homeRank, destinationFile),
                      "Every square between king and rook must be empty, including queenside b-file");
            }

            for (const int file : {4, rookDestination, destinationFile}) {
                auto attacked = castlingPosition(color);
                attacked.board.setPiece(file, attackRank, Piece{PieceType::Rook, opponent});
                check(!hasCastle(rules::generatePseudoLegalMoves(attacked), homeRank, destinationFile),
                      "Cannot castle out of, through, or into check");
            }

            auto rookAttacked = castlingPosition(color);
            rookAttacked.board.setPiece(rookFile, attackRank, Piece{PieceType::Rook, opponent});
            check(hasCastle(rules::generateLegalMoves(rookAttacked), homeRank, destinationFile),
                  "An attacked rook does not prevent castling");
        }

        auto bFileAttacked = castlingPosition(color);
        bFileAttacked.board.setPiece(1, attackRank, Piece{PieceType::Rook, opponent});
        check(hasCastle(rules::generateLegalMoves(bFileAttacked), homeRank, 2),
              "An attacked b-file square does not prevent queenside castling");

        auto kingReturned = castlingPosition(color);
        const int nextRank = color == Color::White ? 1 : 6;
        applyMove(kingReturned, Move{{4, homeRank}, {4, nextRank}});
        kingReturned.sideToMove = color;
        applyMove(kingReturned, Move{{4, nextRank}, {4, homeRank}});
        kingReturned.sideToMove = color;
        check(!hasCastle(rules::generateLegalMoves(kingReturned), homeRank, 2) &&
              !hasCastle(rules::generateLegalMoves(kingReturned), homeRank, 6),
              "Returning king to original square does not restore rights");
    }

    if (failures != 0) {
        return 1;
    }
    std::cout << "All castling checks passed.\n";
}
