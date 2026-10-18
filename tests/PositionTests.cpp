#include "../include/Position.h"

#include <array>
#include <iostream>

std::array<bool, 4> castlingRights(const Position& position) {
    return {position.whiteCanCastleKingside, position.whiteCanCastleQueenside,
            position.blackCanCastleKingside, position.blackCanCastleQueenside};
}

int main() {
    int failures = 0;
    auto check = [&failures](bool condition, const char* name) {
        if (!condition) {
            std::cerr << name << '\n';
            ++failures;
        }
    };

    auto position = createStartingPosition();
    position.halfmoveClock = 7;
    applyMove(position, Move{{4, 1}, {4, 3}}); // e2e4
    check(position.board.at(4, 1).type == PieceType::None, "Move clears source");
    check(position.board.at(4, 3).type == PieceType::Pawn &&
          position.board.at(4, 3).color == Color::White, "Move places piece at destination");
    check(position.sideToMove == Color::Black, "White move passes turn to Black");
    check(position.enPassantTarget && position.enPassantTarget->file == 4 &&
          position.enPassantTarget->rank == 2, "e2e4 sets en passant target to e3");
    check(position.halfmoveClock == 0 && position.fullmoveNumber == 1,
          "White pawn move resets halfmove clock without advancing fullmove number");
    check(castlingRights(position) == std::array{true, true, true, true},
          "Pawn move preserves castling rights");

    applyMove(position, Move{{3, 6}, {3, 4}}); // d7d5
    check(position.sideToMove == Color::White && position.fullmoveNumber == 2,
          "Black move advances fullmove number and passes turn to White");
    check(position.enPassantTarget && position.enPassantTarget->file == 3 &&
          position.enPassantTarget->rank == 5, "d7d5 replaces en passant target with d6");

    applyMove(position, Move{{4, 3}, {3, 4}}); // e4xd5
    check(position.board.at(4, 3).type == PieceType::None &&
          position.board.at(3, 4).type == PieceType::Pawn &&
          position.board.at(3, 4).color == Color::White,
          "Capture replaces enemy piece and clears source");
    check(!position.enPassantTarget, "Pawn capture clears old en passant target");

    applyMove(position, Move{{6, 7}, {5, 5}}); // g8f6
    applyMove(position, Move{{6, 0}, {5, 2}}); // g1f3
    check(position.halfmoveClock == 2 && position.fullmoveNumber == 3,
          "Quiet piece moves increment halfmove clock; only Black advances fullmove number");

    auto expiry = createStartingPosition();
    applyMove(expiry, Move{{4, 1}, {4, 3}});
    applyMove(expiry, Move{{6, 7}, {5, 5}});
    check(!expiry.enPassantTarget, "Quiet piece move expires en passant opportunity");

    for (const auto color : {Color::White, Color::Black}) {
        Position single;
        single.sideToMove = color;
        single.halfmoveClock = 9;
        const int fromRank = color == Color::White ? 1 : 6;
        const int toRank = color == Color::White ? 2 : 5;
        single.board.setPiece(2, fromRank, Piece{PieceType::Pawn, color});
        applyMove(single, Move{{2, fromRank}, {2, toRank}});
        check(!single.enPassantTarget && single.halfmoveClock == 0,
              "Single pawn advance resets clock without creating en passant target");

        auto king = createStartingPosition();
        king.sideToMove = color;
        const int homeRank = color == Color::White ? 0 : 7;
        king.board.setPiece(4, homeRank == 0 ? 1 : 6, Piece{});
        applyMove(king, Move{{4, homeRank}, {4, homeRank == 0 ? 1 : 6}});
        const auto expected = color == Color::White
            ? std::array{false, false, true, true}
            : std::array{true, true, false, false};
        check(castlingRights(king) == expected, "King move removes only its own castling rights");
    }

    struct RookCase {
        Color color;
        Square corner;
        std::array<bool, 4> remainingRights;
    };
    const std::array rookCases{
        RookCase{Color::White, {0, 0}, {true, false, true, true}},
        RookCase{Color::White, {7, 0}, {false, true, true, true}},
        RookCase{Color::Black, {0, 7}, {true, true, true, false}},
        RookCase{Color::Black, {7, 7}, {true, true, false, true}}
    };
    for (const auto& test : rookCases) {
        auto rook = createStartingPosition();
        rook.sideToMove = test.color;
        const Square destination{test.corner.file, test.color == Color::White ? 1 : 6};
        rook.board.setPiece(destination.file, destination.rank, Piece{});
        applyMove(rook, Move{test.corner, destination});
        check(castlingRights(rook) == test.remainingRights,
              "Rook move removes corresponding corner's castling right");
        rook.sideToMove = test.color;
        applyMove(rook, Move{destination, test.corner});
        check(castlingRights(rook) == test.remainingRights,
              "Returning rook to corner does not restore castling rights");

        auto capture = createStartingPosition();
        capture.board.clear();
        capture.sideToMove = test.color == Color::White ? Color::Black : Color::White;
        capture.halfmoveClock = 12;
        capture.board.setPiece(test.corner.file, test.corner.rank, Piece{PieceType::Rook, test.color});
        const Square attacker{test.corner.file == 0 ? 1 : 6, test.corner.rank};
        capture.board.setPiece(attacker.file, attacker.rank,
                               Piece{PieceType::Queen, capture.sideToMove});
        applyMove(capture, Move{attacker, test.corner});
        check(castlingRights(capture) == test.remainingRights,
              "Capturing rook on corner removes corresponding castling right");
        check(capture.halfmoveClock == 0, "Non-pawn capture resets halfmove clock");
    }

    auto otherRook = createStartingPosition();
    otherRook.board.clear();
    otherRook.board.setPiece(3, 3, Piece{PieceType::Rook, Color::White});
    applyMove(otherRook, Move{{3, 3}, {3, 4}});
    check(castlingRights(otherRook) == std::array{true, true, true, true},
          "Rook away from starting corners does not remove castling rights");

    auto bothRooks = createStartingPosition();
    bothRooks.board.clear();
    bothRooks.board.setPiece(0, 0, Piece{PieceType::Rook, Color::White});
    bothRooks.board.setPiece(0, 7, Piece{PieceType::Rook, Color::Black});
    applyMove(bothRooks, Move{{0, 0}, {0, 7}});
    check(castlingRights(bothRooks) == std::array{true, false, true, false},
          "Rook capturing corner rook removes both affected rights");

    if (failures != 0) {
        return 1;
    }
    std::cout << "All ordinary move application checks passed.\n";
}
