#include "Engine.h"
#include "Rules.h"

#include <limits>
#include <stdexcept>
#include <algorithm>
#include <array>

static constexpr std::array<int,64> knightPieceSquareValues = {
    -50,-40,-30,-30,-30,-30,-40,-50,
  -40,-20,  0,  0,  0,  0,-20,-40,
  -30,  0, 10, 15, 15, 10,  0,-30,
  -30,  5, 15, 20, 20, 15,  5,-30,
  -30,  0, 15, 20, 20, 15,  0,-30,
  -30,  5, 10, 15, 15, 10,  5,-30,
  -40,-20,  0,  5,  5,  0,-20,-40,
  -50,-40,-30,-30,-30,-30,-40,-50,
};

static constexpr std::array<int,64> pawnPieceSquareValues = {
    0,  0,  0,  0,  0,  0,  0,  0,
  50, 50, 50, 50, 50, 50, 50, 50,
  10, 10, 20, 30, 30, 20, 10, 10,
   5,  5, 10, 25, 25, 10,  5,  5,
   0,  0,  0, 20, 20,  0,  0,  0,
   5, -5,-10,  0,  0,-10, -5,  5,
   5, 10, 10,-20,-20, 10, 10,  5,
   0,  0,  0,  0,  0,  0,  0,  0
};
static constexpr std::array<int,64> bishopPieceSquareValues = {
    -20,-10,-10,-10,-10,-10,-10,-20,
 -10,  0,  0,  0,  0,  0,  0,-10,
 -10,  0,  5, 10, 10,  5,  0,-10,
 -10,  5,  5, 10, 10,  5,  5,-10,
 -10,  0, 10, 10, 10, 10,  0,-10,
 -10, 10, 10, 10, 10, 10, 10,-10,
 -10,  5,  0,  0,  0,  0,  5,-10,
 -20,-10,-10,-10,-10,-10,-10,-20,
};
static constexpr std::array<int,64> rookPieceSquareValues = {
    0,  0,  0,  0,  0,  0,  0,  0,
  5, 10, 10, 10, 10, 10, 10,  5,
 -5,  0,  0,  0,  0,  0,  0, -5,
 -5,  0,  0,  0,  0,  0,  0, -5,
 -5,  0,  0,  0,  0,  0,  0, -5,
 -5,  0,  0,  0,  0,  0,  0, -5,
 -5,  0,  0,  0,  0,  0,  0, -5,
  0,  0,  0,  5,  5,  0,  0,  0
};

static constexpr std::array<int,64> queenPieceSquareValues = {
    -20,-10,-10, -5, -5,-10,-10,-20,
  -10,  0,  0,  0,  0,  0,  0,-10,
  -10,  0,  5,  5,  5,  5,  0,-10,
   -5,  0,  5,  5,  5,  5,  0, -5,
    0,  0,  5,  5,  5,  5,  0, -5,
  -10,  5,  5,  5,  5,  5,  0,-10,
  -10,  0,  5,  0,  0,  0,  0,-10,
  -20,-10,-10, -5, -5,-10,-10,-20
};
static constexpr std::array<int,64> kingPieceSquareValues = {
    -30,-40,-40,-50,-50,-40,-40,-30,
   -30,-40,-40,-50,-50,-40,-40,-30,
   -30,-40,-40,-50,-50,-40,-40,-30,
   -30,-40,-40,-50,-50,-40,-40,-30,
   -20,-30,-30,-40,-40,-30,-30,-20,
   -10,-20,-20,-20,-20,-20,-20,-10,
    20, 20,  0,  0,  0,  0, 20, 20,
    20, 30, 10,  0,  0, 10, 30, 20
};



// Choose uniformly from the legal moves
std::optional<Move> Engine::chooseRandomMove(const Position& position) {
    const auto moves = rules::generateLegalMoves(position);
    if (moves.empty()) {
        return std::nullopt;
    }
    std::uniform_int_distribution<std::size_t> distribution(0, moves.size() - 1);
    return moves[distribution(randomGenerator)];
}

std::optional<Move> Engine::chooseBestMove(const Position& position, int depth) {
    if (depth < 1) {
        throw std::invalid_argument("Search depth must be at least 1.");
    }

    auto moves = rules::generateLegalMoves(position);
    if (moves.empty()) {
        return std::nullopt;
    }
    if (rules::isDeadPositionInsufficientMaterial(position)) {
        return std::nullopt;
    }
    if (position.halfmoveClock >= 150) {
        return std::nullopt;
    }

    // Search higher-priority moves first
    std::sort(moves.begin(), moves.end(),
        [&](const Move& a, const Move& b) {
            return movePriority(position, a) > movePriority(position, b);
        });

    std::optional<Move> bestMove;
    int bestScore = std::numeric_limits<int>::min();
    int alpha = -1000000;
    int beta = 1000000;
    // Search each candidate and remember the best move
    for (const auto& move : moves) {
        Position nextPosition = position;
        applyMove(nextPosition, move);
        const int score = -negmax(nextPosition, depth - 1, 1,-beta,-alpha);

        if (!bestMove || score > bestScore) {
            bestScore = score;
            bestMove = move;
        }
        if (score > alpha) {
            alpha = score;
        }
    }

    return bestMove;
}

// Material values in centipawns
int Engine::pieceValue(PieceType type) {
    switch (type) {
        case PieceType::Pawn:
            return 100;
        case PieceType::Knight:
            return 320;
        case PieceType::Bishop:
            return 330;
        case PieceType::Rook:
            return 500;
        case PieceType::Queen:
            return 900;
        case PieceType::King:
        case PieceType::None:
            return 0;
    }
    return 0;
}

int Engine::evaluate(const Position& position) const {
    const int checkBonus = 30;
    int score = 0;

    // Material and check bonuses from White's perspective
    for (int rank = 0; rank < 8; ++rank) {
        for (int file = 0; file < 8; ++file) {
            const Piece& piece = position.board.at(file, rank);
            if (piece.type == PieceType::None) {
                continue;
            }
            const int value = pieceValue(piece.type)+pieceSquareBonus(piece, file, rank);
            if (piece.color == Color::White) {
                score += value;
            } else {
                score -= value;
            }

            // Check bonus for attacking the opponent's king
            if (piece.type == PieceType::King) {
                if (piece.color == Color::White &&
                    rules::isSquareAttacked(position, Square{file, rank}, Color::Black)) {
                    score -= checkBonus;
                }
                if (piece.color == Color::Black &&
                    rules::isSquareAttacked(position, Square{file, rank}, Color::White)) {
                    score += checkBonus;
                }
            }
        }
    }

    // Return the score from the side-to-move perspective
    if (position.sideToMove == Color::Black) {
        return -score;
    }
    return score;
}

int Engine::negmax(const Position& position, int depth, int played,int alpha,int beta) {
    auto moves = rules::generateLegalMoves(position);

    // Checkmate and stalemate
    if (moves.empty()) {
        if (rules::isInCheck(position, position.sideToMove)) {
            return -100000 + played; // Checkmate
        }
        return 0; // Stalemate
    }

    // Dead positions and automatic 75-move draws
    if (rules::isDeadPositionInsufficientMaterial(position)) {
        return 0;
    }
    if (position.halfmoveClock >= 150) {
        return 0;
    }

    // Evaluate only after checking terminal positions
    if (depth == 0) {
        return evaluate(position);
    }

    // Search higher-priority moves first
    std::sort(moves.begin(), moves.end(),
        [&](const Move& a, const Move& b) {
            return movePriority(position, a) > movePriority(position, b);
        });

    int best = std::numeric_limits<int>::min();
    for (const auto& move : moves) {
        Position newPosition = position;
        applyMove(newPosition, move);
        const int score = -negmax(newPosition, depth - 1, played + 1,-beta,-alpha);
        if (score > best) {
            best = score;
        }
        if (score > alpha)
            alpha = score;
        if (alpha >= beta)
            break;
    }
    return best;
}

int Engine::movePriority(const Position &position, const Move &move) const {
        int priority=0;

        const Piece& movingPiece = position.board.at(move.from.file,move.from.rank);

        const Piece& targetPiece = position.board.at(move.to.file,move.to.rank);

        int capturedValue = pieceValue(targetPiece.type);

        if (movingPiece.type == PieceType::Pawn && move.from.file != move.to.file &&
            targetPiece.type == PieceType::None)
                capturedValue = pieceValue(PieceType::Pawn);

        if (capturedValue >0) {
            int attackingValue = pieceValue(movingPiece.type);
            priority = 10*capturedValue-attackingValue;
        }

        if (move.promotion != PieceType::None)
            priority += 10000 + pieceValue(move.promotion);

        return priority;

}

int Engine::pieceSquareBonus(const Piece& piece, int file, int rank) const {
    // Mirror Black's square to White's perspective
    int tableRank = rank;
    if (piece.color == Color::Black) {
        tableRank = 7 - rank;
    }

    // These tables list rank 8 first and rank 1 last
    const int tableRow = 7 - tableRank;
    const int index = tableRow * 8 + file;

    switch (piece.type) {
        case PieceType::Pawn:
            return pawnPieceSquareValues[index];
        case PieceType::Knight:
            return knightPieceSquareValues[index];
        case PieceType::Bishop:
            return bishopPieceSquareValues[index];
        case PieceType::Rook:
            return rookPieceSquareValues[index];
        case PieceType::Queen:
            return queenPieceSquareValues[index];
        case PieceType::King:
            return kingPieceSquareValues[index];
        case PieceType::None:
            return 0;
    }
    return 0;
}
