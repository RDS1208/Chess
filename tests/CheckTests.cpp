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

    auto starting = createStartingPosition();
    for (const auto side : {Color::White, Color::Black}) {
        starting.sideToMove = side;
        check(!rules::isInCheck(starting, Color::White) &&
              !rules::isInCheck(starting, Color::Black),
              "Neither king starts in check, regardless of side to move");
        check(rules::generateLegalMoves(starting).size() == 20,
              "Starting position has 20 legal moves for either side");
    }

    for (const auto color : {Color::White, Color::Black}) {
        const auto opponent = color == Color::White ? Color::Black : Color::White;
        const int kingRank = color == Color::White ? 0 : 7;
        const int attackerRank = color == Color::White ? 7 : 0;
        const int shieldRank = color == Color::White ? 1 : 6;

        Position position;
        position.sideToMove = opponent;
        position.board.setPiece(4, kingRank, Piece{PieceType::King, color});
        position.board.setPiece(0, attackerRank, Piece{PieceType::King, opponent});
        position.board.setPiece(4, attackerRank, Piece{PieceType::Rook, opponent});
        check(rules::isInCheck(position, color),
              "Find requested king and detect enemy rook check independently of turn");
        check(!rules::isInCheck(position, opponent),
              "Check to one king does not imply check to the other");

        position.board.setPiece(4, shieldRank, Piece{PieceType::Bishop, color});
        check(!rules::isInCheck(position, color),
              "Intervening piece shields king from check");

        position.board.setPiece(4, shieldRank, Piece{});
        position.board.setPiece(4, attackerRank, Piece{PieceType::Rook, color});
        check(!rules::isInCheck(position, color),
              "Friendly rook does not put its own king in check");

        Position missingKing;
        missingKing.board.setPiece(0, 0, Piece{PieceType::King, opponent});
        bool threw = false;
        try {
            rules::isInCheck(missingKing, color);
        } catch (const std::logic_error&) {
            threw = true;
        }
        check(threw, "Missing requested king throws std::logic_error");
    }

    Position pinned;
    pinned.board.setPiece(4, 0, Piece{PieceType::King, Color::White}); // e1
    pinned.board.setPiece(4, 1, Piece{PieceType::Knight, Color::White}); // e2
    pinned.board.setPiece(4, 7, Piece{PieceType::Rook, Color::Black}); // e8
    pinned.board.setPiece(0, 7, Piece{PieceType::King, Color::Black}); // a8

    bool knightCanMove = false;
    for (const Move& move : rules::generateLegalMoves(pinned)) {
        if (move.from.file == 4 && move.from.rank == 1) {
            knightCanMove = true;
        }
    }
    check(!knightCanMove, "Pinned knight cannot move and expose its king");

    Position kings;
    kings.board.setPiece(4, 0, Piece{PieceType::King, Color::White}); // e1
    kings.board.setPiece(4, 2, Piece{PieceType::King, Color::Black}); // e3
    // White can move only to d1 or f1; all rank-2 destinations are attacked.
    check(rules::generateLegalMoves(kings).size() == 2,
          "King cannot move next to the opposing king");

    Position checked;
    checked.board.setPiece(4, 0, Piece{PieceType::King, Color::White}); // e1
    checked.board.setPiece(1, 0, Piece{PieceType::Knight, Color::White}); // b1
    checked.board.setPiece(4, 7, Piece{PieceType::Rook, Color::Black}); // e8
    checked.board.setPiece(0, 7, Piece{PieceType::King, Color::Black}); // a8
    check(rules::isInCheck(checked, Color::White), "Test position starts in check");
    const auto escapes = rules::generateLegalMoves(checked);
    check(escapes.size() == 4, "Checked king has four escape moves");
    for (const Move& move : escapes) {
        // Only e1d1, e1f1, e1d2, and e1f2 resolve the check.
        check(move.from.file == 4 && move.from.rank == 0 &&
              (move.to.file == 3 || move.to.file == 5) &&
              (move.to.rank == 0 || move.to.rank == 1),
              "Only moves that resolve check are legal");
    }

    auto original = createStartingPosition();
    applyMove(original, Move{{4, 1}, {4, 3}}); // e2e4 creates an en passant target.
    original.whiteCanCastleQueenside = false;
    original.halfmoveClock = 7;
    original.fullmoveNumber = 12;
    const Position before = original;
    rules::generateLegalMoves(original);

    bool boardUnchanged = true;
    for (int rank = 0; rank < 8; ++rank) {
        for (int file = 0; file < 8; ++file) {
            const Piece& oldPiece = before.board.at(file, rank);
            const Piece& currentPiece = original.board.at(file, rank);
            if (oldPiece.type != currentPiece.type || oldPiece.color != currentPiece.color) {
                boardUnchanged = false;
            }
        }
    }
    check(boardUnchanged, "Generating legal moves preserves all board squares");
    check(original.sideToMove == before.sideToMove &&
          original.halfmoveClock == before.halfmoveClock &&
          original.fullmoveNumber == before.fullmoveNumber,
          "Generating legal moves preserves turn and counters");
    check(original.whiteCanCastleKingside == before.whiteCanCastleKingside &&
          original.whiteCanCastleQueenside == before.whiteCanCastleQueenside &&
          original.blackCanCastleKingside == before.blackCanCastleKingside &&
          original.blackCanCastleQueenside == before.blackCanCastleQueenside,
          "Generating legal moves preserves castling rights");
    check(original.enPassantTarget &&
          original.enPassantTarget->file == before.enPassantTarget->file &&
          original.enPassantTarget->rank == before.enPassantTarget->rank,
          "Generating legal moves preserves en passant target");

    if (failures != 0) {
        return 1;
    }
    std::cout << "All check detection and legal move checks passed.\n";
}
