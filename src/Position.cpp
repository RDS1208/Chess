//
// Created by rds on 10/15/26.
//

#include "Position.h"

#include <stdexcept>

Position createStartingPosition() {
    Position position;
    position.board.resetStartingPosition();
    position.sideToMove = Color::White;
    position.whiteCanCastleKingside = true;
    position.whiteCanCastleQueenside = true;
    position.blackCanCastleKingside = true;
    position.blackCanCastleQueenside = true;
    position.enPassantTarget = std::nullopt;
    position.halfmoveClock = 0;
    position.fullmoveNumber = 1;

    return position;
}

void applyMove(Position& position, const Move& move) {
    // Save copies before modifying the board.
    const Piece movingPiece = position.board.at(move.from.file, move.from.rank);
    Piece capturedPiece = position.board.at(move.to.file, move.to.rank);
    const Color movingSide = position.sideToMove;

    // En passant logic
    bool isEnPassant = false;
    if (movingPiece.type == PieceType::Pawn && capturedPiece.type == PieceType::None &&
        move.from.file != move.to.file && position.enPassantTarget) {
        const Square target = *position.enPassantTarget;
        if (move.to.file == target.file && move.to.rank == target.rank) {
            isEnPassant = true;
            capturedPiece = position.board.at(move.to.file, move.from.rank);
        }
    }

    // Promotion logic
    Piece destinationPiece = movingPiece;
    const int promotionRank = movingPiece.color == Color::White ? 7 : 0;
    if (movingPiece.type == PieceType::Pawn && move.to.rank == promotionRank) {
        if (move.promotion != PieceType::Queen && move.promotion != PieceType::Rook &&
            move.promotion != PieceType::Bishop && move.promotion != PieceType::Knight) {
            throw std::invalid_argument("Promotion requires a queen, rook, bishop, or knight.");
        }
        destinationPiece.type = move.promotion;
    } else if (move.promotion != PieceType::None) {
        throw std::invalid_argument("Only a pawn reaching its final rank can promote.");
    }
    // Move the piece
    position.board.setPiece(move.to.file, move.to.rank, destinationPiece);
    position.board.setPiece(move.from.file, move.from.rank, Piece{});

    // Remove the en passant pawn
    if (isEnPassant) {
        position.board.setPiece(move.to.file, move.from.rank, Piece{});
    }

    // Castling logic
    if (movingPiece.type == PieceType::King) {
        const int homeRank = movingPiece.color == Color::White ? 0 : 7;
        if (move.from.file == 4 && move.from.rank == homeRank && move.to.rank == homeRank) {
            if (move.to.file == 6) {
                // Kingside: h-file rook moves to the f-file.
                const Piece rook = position.board.at(7, homeRank);
                position.board.setPiece(5, homeRank, rook);
                position.board.setPiece(7, homeRank, Piece{});
            } else if (move.to.file == 2) {
                // Queenside: a-file rook moves to the d-file.
                const Piece rook = position.board.at(0, homeRank);
                position.board.setPiece(3, homeRank, rook);
                position.board.setPiece(0, homeRank, Piece{});
            }
        }

        // Update king castling rights
        if (movingPiece.color == Color::White) {
            position.whiteCanCastleKingside = false;
            position.whiteCanCastleQueenside = false;
        } else {
            position.blackCanCastleKingside = false;
            position.blackCanCastleQueenside = false;
        }
    }

    // Update rook castling rights
    if (movingPiece.type == PieceType::Rook) {
        if (movingPiece.color == Color::White && move.from.rank == 0) {
            if (move.from.file == 0) {
                position.whiteCanCastleQueenside = false;
            } else if (move.from.file == 7) {
                position.whiteCanCastleKingside = false;
            }
        } else if (movingPiece.color == Color::Black && move.from.rank == 7) {
            if (move.from.file == 0) {
                position.blackCanCastleQueenside = false;
            } else if (move.from.file == 7) {
                position.blackCanCastleKingside = false;
            }
        }
    }

    // Update castling rights after a rook capture
    if (capturedPiece.type == PieceType::Rook) {
        if (capturedPiece.color == Color::White && move.to.rank == 0) {
            if (move.to.file == 0) {
                position.whiteCanCastleQueenside = false;
            } else if (move.to.file == 7) {
                position.whiteCanCastleKingside = false;
            }
        } else if (capturedPiece.color == Color::Black && move.to.rank == 7) {
            if (move.to.file == 0) {
                position.blackCanCastleQueenside = false;
            } else if (move.to.file == 7) {
                position.blackCanCastleKingside = false;
            }
        }
    }

    // Update the en passant target
    position.enPassantTarget = std::nullopt;
    const int rankChange = move.to.rank - move.from.rank;
    if (movingPiece.type == PieceType::Pawn && (rankChange == 2 || rankChange == -2)) {
        position.enPassantTarget = Square{
            move.from.file, (move.from.rank + move.to.rank) / 2
        };
    }

    // Update move counters
    if (movingPiece.type == PieceType::Pawn || capturedPiece.type != PieceType::None) {
        position.halfmoveClock = 0;
    } else {
        ++position.halfmoveClock;
    }
    if (movingSide == Color::Black) {
        ++position.fullmoveNumber;
    }

    // Switch the turn
    position.sideToMove = movingSide == Color::White ? Color::Black : Color::White;
}
