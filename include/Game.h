//
// Created by rds on 10/18/26.
//

#ifndef CHESS_GAME_H
#define CHESS_GAME_H

#include "Move.h"
#include "Position.h"

#include <iosfwd>
#include <string>
#include <vector>

enum class GameStatus {
    Playing,
    Check,
    Checkmate,
    Stalemate,
    DrawByAgreement,
    DrawByRepetition,
    DrawByFiftyMoveRule,
    DrawByFivefoldRepetition,
    DrawBySeventyFiveMoveRule
};

class Game {
public:
    Game();
    explicit Game(const Position& startingPosition);

    // Console game loop
    void run(std::istream& input, std::ostream& output);

    // Draw the board and show whose turn it is
    void drawBoard(std::ostream& output) const;

    // Validate and apply a move; explain rejection through error
    bool tryMove(const Move& move, std::string& error);

    // Record a draw after both players agree; refuse if the game is over
    bool agreeDraw();

    // Accept the draw offered when the current position has three occurrences
    bool canClaimThreefoldDraw() const;
    bool claimThreefoldDraw();

    // Accept the draw offered when the current halfmove counter reaches 100
    bool canClaimFiftyMoveDraw() const;
    bool claimFiftyMoveDraw();

    // Clear a claimed/agreed draw and restore the previous position when available
    bool undo();

    // Check, checkmate, stalemate, claimed/agreed draw, or ordinary play
    GameStatus getStatus() const;

    // Read the current position
    const Position& getPosition() const;

private:
    Position position;
    std::vector<Position> positionhistory;
    GameStatus drawResult = GameStatus::Playing;
};

#endif //CHESS_GAME_H
