#ifndef CHESS_ENGINE_H
#define CHESS_ENGINE_H

#include "Position.h"
#include <optional>
#include <random>

class Engine {
public:
    // Positive scores favor the player whose turn it is; one pawn is 100
    int evaluate(const Position& position) const;
    std::optional<Move> chooseRandomMove(const Position& position);
    std::optional<Move> chooseBestMove(const Position& position, int depth);

private:
    static int pieceValue(PieceType type);
    int pieceSquareBonus(const Piece& piece, int file, int rank) const;
    std::mt19937 randomGenerator{std::random_device{}()};
    int negmax(const Position& position, int depth, int played,int alpha, int beta);
    int movePriority(const Position& position, const Move& move) const;
};

#endif // CHESS_ENGINE_H
