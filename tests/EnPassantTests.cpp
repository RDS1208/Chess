#include "../include/Rules.h"

#include <iostream>

bool hasMove(const std::vector<Move>& moves, const Move& wanted) {
    for (const Move& move : moves) {
        if (move.from.file == wanted.from.file && move.from.rank == wanted.from.rank &&
            move.to.file == wanted.to.file && move.to.rank == wanted.to.rank &&
            move.promotion == wanted.promotion) {
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
        const auto opponent = color == Color::White ? Color::Black : Color::White;
        const int pawnRank = color == Color::White ? 4 : 3;
        const int targetRank = color == Color::White ? 5 : 2;
        const int opponentStartRank = color == Color::White ? 6 : 1;

        for (const int captureFile : {3, 5}) {
            Position position;
            position.sideToMove = opponent;
            position.board.setPiece(0, 0, Piece{PieceType::King, Color::White});
            position.board.setPiece(7, 7, Piece{PieceType::King, Color::Black});
            position.board.setPiece(4, pawnRank, Piece{PieceType::Pawn, color});
            position.board.setPiece(captureFile, opponentStartRank, Piece{PieceType::Pawn, opponent});
            applyMove(position, Move{{captureFile, opponentStartRank}, {captureFile, pawnRank}});

            const Move capture{{4, pawnRank}, {captureFile, targetRank}};
            check(hasMove(rules::generatePseudoLegalMoves(position), capture),
                  "En passant is generated after an adjacent pawn's double advance");
            check(hasMove(rules::generateLegalMoves(position), capture),
                  "En passant is legal for either color and either diagonal");

            Position result = position;
            result.halfmoveClock = 7;
            applyMove(result, capture);
            check(result.board.at(4, pawnRank).type == PieceType::None &&
                  result.board.at(captureFile, pawnRank).type == PieceType::None &&
                  result.board.at(captureFile, targetRank).type == PieceType::Pawn &&
                  result.board.at(captureFile, targetRank).color == color,
                  "En passant moves the pawn and removes the captured pawn from its actual square");
            check(!result.enPassantTarget && result.halfmoveClock == 0 &&
                  result.sideToMove == opponent,
                  "En passant expires its target, resets the clock, and switches turn");

            Position expired = position;
            if (color == Color::White) {
                applyMove(expired, Move{{0, 0}, {1, 0}});
                applyMove(expired, Move{{7, 7}, {6, 7}});
            } else {
                applyMove(expired, Move{{7, 7}, {6, 7}});
                applyMove(expired, Move{{0, 0}, {1, 0}});
            }
            check(!hasMove(rules::generateLegalMoves(expired), capture),
                  "En passant is unavailable after choosing another move");

            Position missingPawn = position;
            missingPawn.board.setPiece(captureFile, pawnRank, Piece{});
            check(!hasMove(rules::generatePseudoLegalMoves(missingPawn), capture),
                  "An en passant target alone is insufficient without the opposing pawn");
            missingPawn.board.setPiece(captureFile, pawnRank, Piece{PieceType::Pawn, color});
            check(!hasMove(rules::generatePseudoLegalMoves(missingPawn), capture),
                  "En passant cannot capture a friendly pawn");

            Position occupied = position;
            occupied.board.setPiece(captureFile, targetRank, Piece{PieceType::Knight, color});
            check(!hasMove(rules::generatePseudoLegalMoves(occupied), capture),
                  "En passant destination must be empty");

            // A normal capture on the target square must not remove the adjacent pawn.
            occupied.board.setPiece(captureFile, targetRank, Piece{PieceType::Knight, opponent});
            applyMove(occupied, capture);
            check(occupied.board.at(captureFile, pawnRank).type == PieceType::Pawn &&
                  occupied.board.at(captureFile, pawnRank).color == opponent,
                  "Ordinary diagonal capture leaves the adjacent pawn in place");
        }
    }

    Position exposed;
    exposed.board.setPiece(4, 4, Piece{PieceType::King, Color::White}); // e5
    exposed.board.setPiece(5, 4, Piece{PieceType::Pawn, Color::White}); // f5
    exposed.board.setPiece(6, 4, Piece{PieceType::Pawn, Color::Black}); // g5
    exposed.board.setPiece(7, 4, Piece{PieceType::Rook, Color::Black}); // h5
    exposed.board.setPiece(0, 7, Piece{PieceType::King, Color::Black}); // a8
    exposed.enPassantTarget = Square{6, 5}; // g6
    const Move unsafeCapture{{5, 4}, {6, 5}};
    check(hasMove(rules::generatePseudoLegalMoves(exposed), unsafeCapture),
          "En passant exposing king is initially pseudo-legal");
    check(!hasMove(rules::generateLegalMoves(exposed), unsafeCapture),
          "En passant exposing king to a rook is rejected by legal-move filtering");

    if (failures != 0) {
        return 1;
    }
    std::cout << "All en passant checks passed.\n";
}
