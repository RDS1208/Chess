//
// Created by rds on 10/15/26.
//

#include "Position.h"

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