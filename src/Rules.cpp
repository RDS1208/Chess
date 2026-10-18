//
// Created by rds on 10/15/26.
//

#include "Rules.h"

#include <cstdlib>
#include <stdexcept>

// Promotion check
bool rules::canPromote(const Piece& piece, Square destination) {
    if (piece.type != PieceType::Pawn) {
        return false;
    }
    if (piece.color == Color::White) {
        return destination.rank == 7;
    }
    return destination.rank == 0;
}

// Pawn moves and promotion choices
void addPawnMove(std::vector<Move>& moves, const Piece& pawn, const Move& move) {
    if (rules::canPromote(pawn, move.to)) {
        // On the final rank, add only the four promotion choices.
        moves.push_back(Move{move.from, move.to, PieceType::Queen});
        moves.push_back(Move{move.from, move.to, PieceType::Rook});
        moves.push_back(Move{move.from, move.to, PieceType::Bishop});
        moves.push_back(Move{move.from, move.to, PieceType::Knight});
    } else {
        // Everywhere else, add one ordinary pawn move.
        moves.push_back(move);
    }
}

std::vector<Move> rules::generatePseudoLegalMoves(const Position& position) {
    std::vector<Move> moves;
    for (int rank=0; rank<8; ++rank) {
        for (int file=0; file<8; ++file) {
            const Piece& piece = position.board.at(file,rank);

            if (piece.type == PieceType::None || piece.color != position.sideToMove) continue;

            // Knight moves
            if (piece.type == PieceType::Knight) {

                constexpr int knightMoves[8][2] = {
                    {1, 2}, {2, 1}, {2, -1}, {1, -2},
                    {-1, -2}, {-2, -1}, {-2, 1}, {-1, 2}
                };

                for (const auto& move : knightMoves) {
                    const int targetFile = file + move[0];
                    const int targetRank = rank + move[1];

                    if (targetFile < 0 || targetFile >= 8 ||
                        targetRank < 0 || targetRank >= 8) {
                        continue;
                    }

                    const Piece& target = position.board.at(targetFile, targetRank);
                    if (target.type != PieceType::None && target.color == piece.color) {
                        continue;
                    }
                    if (target.type == PieceType::King) {
                        continue;
                    }

                    moves.push_back(Move{
                        Square{file, rank},
                        Square{targetFile, targetRank}
                    });
                }
            }

            // Rook moves
            if (piece.type == PieceType::Rook) {
                constexpr int directions[4][2] = {
                    {1, 0}, {-1, 0}, {0, 1}, {0, -1}
                };

                for (const auto& dir : directions) {
                    int targetFile = file;
                    int targetRank = rank;

                    while (true) {
                        targetFile += dir[0];
                        targetRank += dir[1];

                        if (targetFile < 0 || targetFile >= 8 ||
                            targetRank < 0 || targetRank >= 8) {
                            break;
                        }

                        const Piece& target = position.board.at(targetFile, targetRank);
                        if (target.type != PieceType::None) {
                            if (target.color != piece.color && target.type != PieceType::King) {
                                moves.push_back(Move{
                                    Square{file, rank},
                                    Square{targetFile, targetRank}
                                });
                            }
                            break;
                        }

                        moves.push_back(Move{
                            Square{file, rank},
                            Square{targetFile, targetRank}
                        });
                    }
                }
            }
            // Bishop moves
            if (piece.type == PieceType::Bishop) {
                constexpr int directions[4][2] = {
                    {1, 1}, {1, -1}, {-1, 1}, {-1, -1}
                };
                for (const auto& dir : directions) {
                    int targetFile = file;
                    int targetRank = rank;

                    while (true) {
                        targetFile += dir[0];
                        targetRank += dir[1];

                        if (targetFile < 0 || targetFile >= 8 ||
                            targetRank < 0 || targetRank >= 8) {
                            break;
                        }

                        const Piece& target = position.board.at(targetFile, targetRank);
                        if (target.type != PieceType::None) {
                            if (target.color != piece.color && target.type != PieceType::King) {
                                moves.push_back(Move{
                                    Square{file, rank},
                                    Square{targetFile, targetRank}
                                });
                            }
                            break;
                        }

                        moves.push_back(Move{
                            Square{file, rank},
                            Square{targetFile, targetRank}
                        });
                    }
                }
            }
            // Queen moves
            if (piece.type == PieceType::Queen) {
                constexpr int directions[8][2] = {
                    {1, 0}, {-1, 0}, {0, 1}, {0, -1},
                    {1, 1}, {1, -1}, {-1, 1}, {-1, -1}
                };
                for (const auto& dir : directions) {
                    int targetFile = file;
                    int targetRank = rank;

                    while (true) {
                        targetFile += dir[0];
                        targetRank += dir[1];

                        if (targetFile < 0 || targetFile >= 8 ||
                            targetRank < 0 || targetRank >= 8) {
                            break;
                        }

                        const Piece& target = position.board.at(targetFile, targetRank);
                        if (target.type != PieceType::None) {
                            if (target.color != piece.color && target.type != PieceType::King) {
                                moves.push_back(Move{
                                    Square{file, rank},
                                    Square{targetFile, targetRank}
                                });
                            }
                            break;
                        }

                        moves.push_back(Move{
                            Square{file, rank},
                            Square{targetFile, targetRank}
                        });
                    }
                }
            }
            // King moves
            if (piece.type == PieceType::King) {
                constexpr int kingMoves[8][2] = {
                    {1, 0}, {-1, 0}, {0, 1}, {0, -1},
                    {1, 1}, {1, -1}, {-1, 1}, {-1, -1}
                };

                for (const auto& move : kingMoves) {
                    const int targetFile = file + move[0];
                    const int targetRank = rank + move[1];

                    if (targetFile < 0 || targetFile >= 8 ||
                        targetRank < 0 || targetRank >= 8) {
                        continue;
                    }

                    const Piece& target = position.board.at(targetFile, targetRank);
                    if (target.type != PieceType::None && target.color == piece.color) {
                        continue;
                    }
                    if (target.type == PieceType::King) {
                        continue;
                    }

                    moves.push_back(Move{
                        Square{file, rank},
                        Square{targetFile, targetRank}
                    });
                }
            }
            // Castling logic
            if (piece.type == PieceType::King) {
                int homeRank;
                Color opponent;
                bool kingsideRight;
                bool queensideRight;
                if (piece.color == Color::White) {
                    homeRank = 0;
                    opponent = Color::Black;
                    kingsideRight = position.whiteCanCastleKingside;
                    queensideRight = position.whiteCanCastleQueenside;
                } else {
                    homeRank = 7;
                    opponent = Color::White;
                    kingsideRight = position.blackCanCastleKingside;
                    queensideRight = position.blackCanCastleQueenside;
                }

                // The king must start on its original square and cannot castle out of check.
                if (file == 4 && rank == homeRank &&
                    !isSquareAttacked(position, Square{4, homeRank}, opponent)) {
                    if (kingsideRight) {
                        const Piece& rook = position.board.at(7, homeRank);
                        if (rook.type == PieceType::Rook && rook.color == piece.color &&
                            position.board.at(5, homeRank).type == PieceType::None &&
                            position.board.at(6, homeRank).type == PieceType::None &&
                            !isSquareAttacked(position, Square{5, homeRank}, opponent) &&
                            !isSquareAttacked(position, Square{6, homeRank}, opponent)) {
                            moves.push_back(Move{Square{4, homeRank}, Square{6, homeRank}});
                        }
                    }

                    if (queensideRight) {
                        const Piece& rook = position.board.at(0, homeRank);
                        if (rook.type == PieceType::Rook && rook.color == piece.color &&
                            position.board.at(1, homeRank).type == PieceType::None &&
                            position.board.at(2, homeRank).type == PieceType::None &&
                            position.board.at(3, homeRank).type == PieceType::None &&
                            !isSquareAttacked(position, Square{3, homeRank}, opponent) &&
                            !isSquareAttacked(position, Square{2, homeRank}, opponent)) {
                            moves.push_back(Move{Square{4, homeRank}, Square{2, homeRank}});
                        }
                    }
                }
            }
            // Pawn moves
            if (piece.type == PieceType::Pawn) {
                const int direction = (piece.color == Color::White) ? 1 : -1;
                const int startRank = (piece.color == Color::White) ? 1 : 6;

                if (rank + direction >= 0 && rank + direction < 8) {
                    const Piece& forward = position.board.at(file, rank + direction);
                    if (forward.type == PieceType::None) {
                        addPawnMove(moves, piece, Move{
                            Square{file, rank},
                            Square{file, rank + direction}
                        });

                        if (rank == startRank) {
                            const Piece& doubleForward = position.board.at(file, rank + 2 * direction);
                            if (doubleForward.type == PieceType::None) {
                                moves.push_back(Move{
                                    Square{file, rank},
                                    Square{file, rank + 2 * direction}
                                });
                            }
                        }
                    }

                    constexpr int captureOffsets[2] = {-1, 1};
                    for (const int fileOffset : captureOffsets) {
                        const int targetFile = file + fileOffset;
                        const int targetRank = rank + direction;
                        if (targetFile < 0 || targetFile >= 8) {
                            continue;
                        }

                        const Piece& target = position.board.at(targetFile, targetRank);
                        if (target.type != PieceType::None && target.color != piece.color &&
                            target.type != PieceType::King) {
                            addPawnMove(moves, piece, Move{
                                Square{file, rank},
                                Square{targetFile, targetRank}
                            });
                        } else if (target.type == PieceType::None && position.enPassantTarget) {
                            // En passant logic
                            const Square enPassant = *position.enPassantTarget;
                            if (targetFile == enPassant.file && targetRank == enPassant.rank) {
                                const Piece& capturedPawn = position.board.at(targetFile, rank);
                                if (capturedPawn.type == PieceType::Pawn &&
                                    capturedPawn.color != piece.color) {
                                    moves.push_back(Move{
                                        Square{file, rank},
                                        Square{targetFile, targetRank}
                                    });
                                }
                            }
                        }
                    }
                }
            }
        }

    }

    return moves;
}

// Attack detection
bool rules::isSquareAttacked(const Position& position, Square square, Color attackingColor) {
    for (int rank = 0; rank < 8; ++rank) {
        for (int file = 0; file < 8; ++file) {
            const Piece& piece = position.board.at(file, rank);
            if (piece.type == PieceType::None || piece.color != attackingColor) {
                continue;
            }

            const int fileDifference = square.file - file;
            const int rankDifference = square.rank - rank;
            const int fileDistance = std::abs(fileDifference);
            const int rankDistance = std::abs(rankDifference);

            // A piece does not attack its own square.
            if (fileDistance == 0 && rankDistance == 0) {
                continue;
            }

            if (piece.type == PieceType::Pawn) {
                const int direction = piece.color == Color::White ? 1 : -1;
                if (fileDistance == 1 && rankDifference == direction) {
                    return true;
                }
            }

            if (piece.type == PieceType::Knight) {
                if ((fileDistance == 1 && rankDistance == 2) ||
                    (fileDistance == 2 && rankDistance == 1)) {
                    return true;
                }
            }

            if (piece.type == PieceType::King) {
                if (fileDistance <= 1 && rankDistance <= 1) {
                    return true;
                }
            }

            const bool straight = fileDifference == 0 || rankDifference == 0;
            const bool diagonal = fileDistance == rankDistance;
            const bool slidingAttack =
                (piece.type == PieceType::Rook && straight) ||
                (piece.type == PieceType::Bishop && diagonal) ||
                (piece.type == PieceType::Queen && (straight || diagonal));
            if (!slidingAttack) {
                continue;
            }

            // Each step is -1, 0, or 1, pointing toward the target square.
            int fileStep = 0;
            if (fileDifference > 0) {
                fileStep = 1;
            } else if (fileDifference < 0) {
                fileStep = -1;
            }

            int rankStep = 0;
            if (rankDifference > 0) {
                rankStep = 1;
            } else if (rankDifference < 0) {
                rankStep = -1;
            }

            int currentFile = file + fileStep;
            int currentRank = rank + rankStep;
            bool blocked = false;

            // Only intervening pieces block: the target can be occupied by either color.
            while (currentFile != square.file || currentRank != square.rank) {
                if (position.board.at(currentFile, currentRank).type != PieceType::None) {
                    blocked = true;
                    break;
                }
                currentFile += fileStep;
                currentRank += rankStep;
            }
            if (!blocked) {
                return true;
            }
        }
    }
    return false;
}

// Check detection
bool rules::isInCheck(const Position& position, Color color) {
    Color opponentColor;
    if (color == Color::White) {
        opponentColor = Color::Black;
    } else {
        opponentColor = Color::White;
    }

    for (int rank = 0; rank < 8; ++rank) {
        for (int file = 0; file < 8; ++file) {
            const Piece& piece = position.board.at(file, rank);
            if (piece.type == PieceType::King && piece.color == color) {
                return isSquareAttacked(position, Square{file, rank}, opponentColor);
            }
        }
    }

    throw std::logic_error("Cannot detect check: requested king is missing from the position.");
}

// Legal move filtering
std::vector<Move> rules::generateLegalMoves(const Position& position) {
    std::vector<Move> pseudoLegalMoves = rules::generatePseudoLegalMoves(position);
    std::vector<Move> legalMoves;

    for (const Move& move : pseudoLegalMoves) {
        Position newPosition = position;
        applyMove(newPosition, move);
        if (!rules::isInCheck(newPosition, position.sideToMove)) {
            legalMoves.push_back(move);
        }
    }



    return legalMoves;
}

// A stored target matters for repetition only when en passant is legal
std::optional<Square> legalEnPassantTarget(const Position& position) {
    if (!position.enPassantTarget) {
        return std::nullopt;
    }
    const Square target = *position.enPassantTarget;
    for (const Move& move : rules::generateLegalMoves(position)) {
        const Piece& piece = position.board.at(move.from.file, move.from.rank);
        if (piece.type == PieceType::Pawn && move.from.file != move.to.file &&
            move.to.file == target.file && move.to.rank == target.rank &&
            position.board.at(target.file, target.rank).type == PieceType::None) {
            return target;
        }
    }
    return std::nullopt;
}

bool rules::isSamePosition(const Position& a, const Position& b) {
    // Compare pieces on every square
    for (int rank = 0; rank < 8; ++rank) {
        for (int file = 0; file < 8; ++file) {
            const Piece& first = a.board.at(file, rank);
            const Piece& second = b.board.at(file, rank);

            if (first.type != second.type) {
                return false;
            }

            if (first.type != PieceType::None && first.color != second.color) {
                return false;
            }
        }
    }

    // Compare en passant targets only when a legal capture exists
    const auto firstTarget = legalEnPassantTarget(a);
    const auto secondTarget = legalEnPassantTarget(b);
    if (firstTarget.has_value() != secondTarget.has_value()) {
        return false;
    }
    if (firstTarget &&
        (firstTarget->file != secondTarget->file || firstTarget->rank != secondTarget->rank)) {
        return false;
    }

    return a.sideToMove == b.sideToMove &&
           a.whiteCanCastleKingside == b.whiteCanCastleKingside &&
           a.whiteCanCastleQueenside == b.whiteCanCastleQueenside &&
           a.blackCanCastleKingside == b.blackCanCastleKingside &&
           a.blackCanCastleQueenside == b.blackCanCastleQueenside;
}

int rules::countRepetitions(const std::vector<Position>& history, const Position& current) {
    int count = 1; // Count the current position as well as previous occurrences
    for (const Position& pastPosition : history) {
        if (rules::isSamePosition(pastPosition, current)) {
            ++count;
        }
    }
    return count;
}

bool rules::threeFoldRepetition(const std::vector<Position>& history, const Position& current) {
    return countRepetitions(history, current) >= 3;
}

bool rules::fiveFoldRepetition(const std::vector<Position>& history, const Position& current) {
    return countRepetitions(history, current) >= 5;
}

bool rules::isDeadPosition(const Position &position) {
    bool whiteHasMaterial = false;
    bool blackHasMaterial = false;
    for (int rank =0 ; rank < 8 ; ++rank)
    {
        for (int file = 0 ; file < 8 ; ++file)
        {
            const Piece& piece = position.board.at(file, rank);
            if (piece.type == PieceType::None || piece.type == PieceType::King) continue;
            if (piece.color == Color::White) {
                whiteHasMaterial = true;
            } else {
                blackHasMaterial = true;
            }
        }
    }
    return !whiteHasMaterial || !blackHasMaterial;
}
