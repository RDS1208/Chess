//
// Created by rds on 10/18/26.
//

#include "Game.h"

#include "Console.h"
#include "Rules.h"

#include <istream>
#include <ostream>

// Starting position
Game::Game() : position(createStartingPosition()) {
}

// Start a game from a supplied position
Game::Game(const Position& startingPosition) : position(startingPosition) {
}

// Draw the board and show whose turn it is
void Game::drawBoard(std::ostream& output) const {
    printBoard(position.board, output);

    if (position.sideToMove == Color::White) {
        output << "White to move.\n";
    } else {
        output << "Black to move.\n";
    }
}

// Check the state of the player whose turn it is
GameStatus Game::getStatus() const {
    if (drawResult != GameStatus::Playing) {
        return drawResult;
    }

    const auto legalMoves = rules::generateLegalMoves(position);
    const bool isCheck = rules::isInCheck(position, position.sideToMove);

    if (legalMoves.empty()) {
        if (isCheck) {
            return GameStatus::Checkmate;
        }
        return GameStatus::Stalemate;
    }

    // Automatic draws; checkmate above takes precedence
    if (rules::fiveFoldRepetition(positionhistory, position)) {
        return GameStatus::DrawByFivefoldRepetition;
    }
    if (position.halfmoveClock >= 150) {
        return GameStatus::DrawBySeventyFiveMoveRule;
    }

    if (rules::isDeadPositionInsufficientMaterial(position)) {
        return GameStatus::DeadPositionInsufficientMaterial;
    }

    if (isCheck) {
        return GameStatus::Check;
    }
    return GameStatus::Playing;
}

// Read the current position without copying or changing it
const Position& Game::getPosition() const {
    return position;
}

// Call after both players have accepted the draw
bool Game::agreeDraw() {
    const GameStatus status = getStatus();
    if (status != GameStatus::Playing && status != GameStatus::Check) {
        return false;
    }
    drawResult = GameStatus::DrawByAgreement;
    return true;
}

// Check whether a threefold claim can be offered
bool Game::canClaimThreefoldDraw() const {
    const GameStatus status = getStatus();
    if (status != GameStatus::Playing && status != GameStatus::Check) {
        return false;
    }
    return rules::threeFoldRepetition(positionhistory, position);
}

// Claim a draw based on the current position and its history
bool Game::claimThreefoldDraw() {
    if (!canClaimThreefoldDraw()) {
        return false;
    }
    drawResult = GameStatus::DrawByRepetition;
    return true;
}

// Check whether a fifty-move claim can be offered
bool Game::canClaimFiftyMoveDraw() const {
    const GameStatus status = getStatus();
    if (status != GameStatus::Playing && status != GameStatus::Check) {
        return false;
    }
    return position.halfmoveClock >= 100;
}

// Fifty-move claim based on the current position
bool Game::claimFiftyMoveDraw() {
    if (!canClaimFiftyMoveDraw()) {
        return false;
    }
    drawResult = GameStatus::DrawByFiftyMoveRule;
    return true;
}

bool Game::undo() {
    if (positionhistory.empty()) {
        if (drawResult != GameStatus::Playing) {
            drawResult = GameStatus::Playing;
            return true;
        }
        return false;
    }
    drawResult = GameStatus::Playing;
    position = positionhistory.back();
    positionhistory.pop_back();
    return true;
}

// Validate a move and save history only when it is accepted
bool Game::tryMove(const Move& move, std::string& error) {
    error.clear();

    // Stop moves after the game ends
    const GameStatus status = getStatus();
    if (status == GameStatus::Checkmate) {
        error = "The game is over: checkmate.";
        return false;
    }
    if (status == GameStatus::Stalemate) {
        error = "The game is over: stalemate.";
        return false;
    }
    if (status == GameStatus::DrawByAgreement) {
        error = "The game is over: draw by agreement.";
        return false;
    }
    if (status == GameStatus::DrawByRepetition) {
        error = "The game is over: draw by threefold repetition.";
        return false;
    }
    if (status == GameStatus::DrawByFiftyMoveRule) {
        error = "The game is over: draw by the 50-move rule.";
        return false;
    }
    if (status == GameStatus::DrawByFivefoldRepetition) {
        error = "The game is over: draw by fivefold repetition.";
        return false;
    }
    if (status == GameStatus::DrawBySeventyFiveMoveRule) {
        error = "The game is over: draw by the 75-move rule.";
        return false;
    }
    if (status == GameStatus::DeadPositionInsufficientMaterial) {
        error = "The game is over: dead position.";
        return false;
    }

    // Check coordinates before reading the board
    if (move.from.file < 0 || move.from.file >= 8 ||
        move.from.rank < 0 || move.from.rank >= 8 ||
        move.to.file < 0 || move.to.file >= 8 ||
        move.to.rank < 0 || move.to.rank >= 8) {
        error = "Move coordinates must be inside the board.";
        return false;
    }

    const Piece& piece = position.board.at(move.from.file, move.from.rank);
    if (piece.type == PieceType::None) {
        error = "There is no piece on the starting square.";
        return false;
    }
    if (piece.color != position.sideToMove) {
        error = "That piece belongs to the other player.";
        return false;
    }
    if (move.from.file == move.to.file && move.from.rank == move.to.rank) {
        error = "The destination must be different from the starting square.";
        return false;
    }

    const Piece& target = position.board.at(move.to.file, move.to.rank);
    if (target.type != PieceType::None && target.color == piece.color) {
        error = "You cannot move onto a square occupied by your own piece.";
        return false;
    }
    if (target.type == PieceType::King) {
        error = "A king cannot be captured.";
        return false;
    }

    // Promotion must select one of the four allowed pieces
    if (rules::canPromote(piece, move.to)) {
        if (move.promotion != PieceType::Queen && move.promotion != PieceType::Rook &&
            move.promotion != PieceType::Bishop && move.promotion != PieceType::Knight) {
            error = "Promotion requires q, r, b, or n, for example e7e8q.";
            return false;
        }
    } else if (move.promotion != PieceType::None) {
        error = "Only a pawn reaching its final rank can promote.";
        return false;
    }

    // Match source, destination, and promotion against the legal moves
    const auto legalMoves = rules::generateLegalMoves(position);
    for (const Move& legalMove : legalMoves) {
        if (legalMove.from.file == move.from.file && legalMove.from.rank == move.from.rank &&
            legalMove.to.file == move.to.file && legalMove.to.rank == move.to.rank &&
            legalMove.promotion == move.promotion) {
            Position newPosition = position;
            applyMove(newPosition, legalMove);
            positionhistory.push_back(position);
            position = newPosition;
            return true;
        }
    }

    // A pseudo-legal move excluded above would leave the king in check
    const auto pseudoLegalMoves = rules::generatePseudoLegalMoves(position);
    for (const Move& candidate : pseudoLegalMoves) {
        if (candidate.from.file == move.from.file && candidate.from.rank == move.from.rank &&
            candidate.to.file == move.to.file && candidate.to.rank == move.to.rank &&
            candidate.promotion == move.promotion) {
            error = "That move would leave your king in check.";
            return false;
        }
    }

    const int fileDifference = move.to.file - move.from.file;
    if (piece.type == PieceType::King && move.from.rank == move.to.rank &&
        (fileDifference == 2 || fileDifference == -2)) {
        error = "Castling requires castling rights, the correct rook, an empty path, and safe king squares.";
    } else {
        error = "That piece cannot move to that square, or its path is blocked.";
    }
    return false;
}

// Console game loop
void Game::run(std::istream& input, std::ostream& output) {
    while (true) {
        drawBoard(output);

        // Report the current game state
        const GameStatus status = getStatus();
        const bool gameOver = status != GameStatus::Playing && status != GameStatus::Check;
        if (status == GameStatus::Checkmate) {
            if (position.sideToMove == Color::White) {
                output << "Checkmate! Black wins.\n";
            } else {
                output << "Checkmate! White wins.\n";
            }
        } else if (status == GameStatus::Stalemate) {
            output << "Stalemate. Draw.\n";
        } else if (status == GameStatus::DrawByAgreement) {
            output << "Draw by agreement.\n";
        } else if (status == GameStatus::DrawByRepetition) {
            output << "Draw by threefold repetition.\n";
        } else if (status == GameStatus::DrawByFiftyMoveRule) {
            output << "Draw by the 50-move rule.\n";
        } else if (status == GameStatus::DrawByFivefoldRepetition) {
            output << "Draw by fivefold repetition.\n";
        } else if (status == GameStatus::DrawBySeventyFiveMoveRule) {
            output << "Draw by the 75-move rule.\n";
        } else if (status == GameStatus::DeadPositionInsufficientMaterial) {
            output << "Dead position: insufficient material to checkmate.\n";
        } else if (status == GameStatus::Check) {
            output << "Check.\n";
        }

        // Offer available claims; the player may continue instead
        const bool threefold = canClaimThreefoldDraw();
        const bool fiftyMoves = canClaimFiftyMoveDraw();
        if (threefold || fiftyMoves) {
            if (threefold) {
                output << "Threefold repetition detected. ";
            } else {
                output << "50-move rule: 100 or more halfmoves without a pawn move or capture. ";
            }
            output << "Do you wish to claim the draw? (y/n): " << std::flush;
            std::string response;
            while (true) {
                if (!std::getline(input, response)) {
                    output << '\n';
                    return;
                }
                if (response == "y" || response == "Y" || response == "n" || response == "N") {
                    break;
                }
                output << "Please enter y or n: " << std::flush;
            }
            if (response == "y" || response == "Y") {
                if (threefold) {
                    claimThreefoldDraw();
                } else {
                    claimFiftyMoveDraw();
                }
                output << "Draw claimed. Game over.\n";
                continue;
            }
            output << "Draw not claimed. Game continues.\n";
        }

        // Read a move or command
        if (gameOver) {
            output << "Game over. Enter undo or quit: ";
        } else {
            output << "Enter a move (e.g. e2e4 or e7e8q), draw, undo, or quit: ";
        }
        output << std::flush;

        std::string text;
        if (!std::getline(input, text)) {
            output << '\n';
            return;
        }

        // Commands are handled before move parsing
        if (text == "quit") {
            output << "Exiting game.\n";
            return;
        }
        if (text == "undo") {
            if (undo()) {
                output << "Undo successful.\n";
            } else {
                output << "No moves to undo.\n";
            }
            continue;
        }

        // After a result, only undo and quit are allowed
        if (gameOver) {
            output << "Game over. Only undo or quit is allowed.\n";
            continue;
        }

        // Draw by agreement
        if (text == "draw") {
            output << "Draw has been offered. Does the other player accept? (y/n): " << std::flush;
            std::string response;
            while (true) {
                if (!std::getline(input, response)) {
                    output << '\n';
                    return;
                }
                if (response == "y" || response == "Y") {
                    agreeDraw();
                    output << "Draw accepted. Game over.\n";
                    break;
                }
                if (response == "n" || response == "N") {
                    output << "Draw declined. Game continues.\n";
                    break;
                }
                output << "Please enter y or n: " << std::flush;
            }
            continue;
        }

        // Parse and validate the move
        const auto move = parseMove(text);
        if (!move) {
            output << "Invalid input. Use e2e4, or a promotion such as e7e8q (q, r, b, or n).\n";
            continue;
        }

        std::string error;
        if (!tryMove(*move, error)) {
            output << error << '\n';
        }
    }
}
