//
// Created by rds on 10/15/26.
//

#include "Rules.h"
std::vector<Move> rules::generatePseudoLegalMoves(const Position& position) {
    std::vector<Move> moves;
    for (int rank=0; rank<8; ++rank) {
        for (int file=0; file<8; ++file) {
            const Piece& piece = position.board.at(file,rank);

            if (piece.type == PieceType::None || piece.color != position.sideToMove) continue;

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
            if (piece.type == PieceType::Pawn) {
                const int direction = (piece.color == Color::White) ? 1 : -1;
                const int startRank = (piece.color == Color::White) ? 1 : 6;

                if (rank + direction >= 0 && rank + direction < 8) {
                    const Piece& forward = position.board.at(file, rank + direction);
                    if (forward.type == PieceType::None) {
                        moves.push_back(Move{
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

    return moves;
}
