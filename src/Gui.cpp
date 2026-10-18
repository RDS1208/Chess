#include "Gui.h"

#include "Rules.h"

// Draw text at a position, optionally centered around that position
void drawGuiText(sf::RenderTarget& target, const sf::Font& font, const sf::String& value,
                 unsigned int size, sf::Vector2f position, sf::Color color, bool centered = false) {
    sf::Text text(font, value, size);
    text.setFillColor(color);
    if (centered) {
        const auto bounds = text.getLocalBounds();
        text.setOrigin(bounds.position + bounds.size / 2.f);
    }
    text.setPosition(position);
    target.draw(text);
}

// Draw a simple button
void drawGuiButton(sf::RenderTarget& target, const sf::Font& font, const std::string& label,
                   float x, float y, float width, bool selected = false) {
    sf::RectangleShape button({width, 38.f});
    button.setPosition({x, y});
    button.setFillColor(sf::Color(53, 62, 73));
    if (selected) {
        button.setFillColor(sf::Color(64, 112, 153));
    }
    button.setOutlineThickness(-1.f);
    button.setOutlineColor(sf::Color(121, 137, 150));
    target.draw(button);
    drawGuiText(target, font, label, 15, {x + width / 2.f, y + 19.f}, sf::Color::White, true);
}

// Chess symbols provided by the bundled font
char32_t pieceGlyph(PieceType type) {
    switch (type) {
        case PieceType::King: return U'\u265A';
        case PieceType::Queen: return U'\u265B';
        case PieceType::Rook: return U'\u265C';
        case PieceType::Bishop: return U'\u265D';
        case PieceType::Knight: return U'\u265E';
        case PieceType::Pawn: return U'\u265F';
        default: return U' ';
    }
}

Gui::Gui(Game& game) : game(game) {
    checkDrawOffer();
}

// Offer a draw when the current position qualifies
void Gui::checkDrawOffer() {
    drawOffer = DrawOffer::None;
    if (game.canClaimThreefoldDraw()) {
        drawOffer = DrawOffer::Threefold;
    } else if (game.canClaimFiftyMoveDraw()) {
        drawOffer = DrawOffer::FiftyMove;
    }
}

// Draw the board, pieces, game status, and controls
void Gui::draw(sf::RenderTarget& target, const sf::Font& font) const {
    const Position& position = game.getPosition();
    const GameStatus status = game.getStatus();
    std::string heading = "White to move";
    if (position.sideToMove == Color::Black) {
        heading = "Black to move";
    }
    if (status == GameStatus::Check) {
        heading += " - Check";
    } else if (status == GameStatus::Checkmate) {
        if (position.sideToMove == Color::White) {
            heading = "Checkmate! Black wins.";
        } else {
            heading = "Checkmate! White wins.";
        }
    } else if (status == GameStatus::Stalemate) {
        heading = "Stalemate. Draw.";
    } else if (status == GameStatus::DrawByAgreement) {
        heading = "Draw by agreement.";
    } else if (status == GameStatus::DrawByRepetition) {
        heading = "Draw by threefold repetition.";
    } else if (status == GameStatus::DrawByFiftyMoveRule) {
        heading = "Draw by the 50-move rule.";
    } else if (status == GameStatus::DrawByFivefoldRepetition) {
        heading = "Draw by fivefold repetition.";
    } else if (status == GameStatus::DrawBySeventyFiveMoveRule) {
        heading = "Draw by the 75-move rule.";
    }
    drawGuiText(target, font, heading, 24, {40.f, 16.f}, sf::Color(242, 240, 233));

    // Keep longer error messages on two lines
    std::string displayedMessage = message;
    if (displayedMessage.size() > 80) {
        const auto split = displayedMessage.rfind(' ', 80);
        if (split != std::string::npos) {
            displayedMessage[split] = '\n';
        }
    }
    drawGuiText(target, font, displayedMessage, 14, {40.f, 52.f}, sf::Color(191, 203, 214));

    // Mark legal destinations for the selected piece
    bool destinations[8][8]{};
    if (selectedSquare) {
        for (const Move& move : rules::generateLegalMoves(position)) {
            if (move.from.file == selectedSquare->file && move.from.rank == selectedSquare->rank) {
                destinations[move.to.file][move.to.rank] = true;
            }
        }
    }

    for (int rank = 0; rank < 8; ++rank) {
        for (int file = 0; file < 8; ++file) {
            const float x = BoardLeft + file * SquareSize;
            const float y = BoardTop + (7 - rank) * SquareSize;
            const Piece& piece = position.board.at(file, rank);

            sf::RectangleShape square({SquareSize, SquareSize});
            square.setPosition({x, y});
            square.setFillColor(sf::Color(181, 136, 99));
            if ((file + rank) % 2 == 1) {
                square.setFillColor(sf::Color(240, 217, 181));
            }
            if ((status == GameStatus::Check || status == GameStatus::Checkmate) &&
                piece.type == PieceType::King && piece.color == position.sideToMove) {
                square.setFillColor(sf::Color(207, 114, 101));
            }
            target.draw(square);

            // Empty destinations use dots; captures use rings
            if (destinations[file][rank]) {
                float radius = 9.f;
                if (piece.type != PieceType::None) {
                    radius = 33.f;
                }
                sf::CircleShape marker(radius);
                marker.setOrigin({radius, radius});
                marker.setPosition({x + 40.f, y + 40.f});
                marker.setFillColor(sf::Color(35, 90, 55, 150));
                if (piece.type != PieceType::None) {
                    marker.setFillColor(sf::Color::Transparent);
                    marker.setOutlineColor(sf::Color(35, 90, 55, 180));
                    marker.setOutlineThickness(4.f);
                }
                target.draw(marker);
            }

            // Use the same silhouette with different colors for each player
            if (piece.type != PieceType::None) {
                sf::Text symbol(font, sf::String(pieceGlyph(piece.type)), 58);
                const auto bounds = symbol.getLocalBounds();
                symbol.setOrigin(bounds.position + bounds.size / 2.f);
                symbol.setPosition({x + 40.f, y + 40.f});
                symbol.setFillColor(sf::Color(37, 39, 43));
                symbol.setOutlineColor(sf::Color(239, 231, 212));
                symbol.setOutlineThickness(1.f);
                if (piece.color == Color::White) {
                    symbol.setFillColor(sf::Color(255, 251, 235));
                    symbol.setOutlineColor(sf::Color(44, 42, 38));
                    symbol.setOutlineThickness(1.5f);
                }
                target.draw(symbol);
            }

            if (selectedSquare && selectedSquare->file == file && selectedSquare->rank == rank) {
                square.setFillColor(sf::Color::Transparent);
                square.setOutlineColor(sf::Color(255, 222, 83));
                square.setOutlineThickness(-4.f);
                target.draw(square);
            }
        }
    }

    // Board coordinates
    for (int index = 0; index < 8; ++index) {
        drawGuiText(target, font, std::string(1, static_cast<char>('a' + index)), 16,
                    {80.f + index * SquareSize, 754.f}, sf::Color(191, 203, 214), true);
        drawGuiText(target, font, std::to_string(8 - index), 16,
                    {21.f, 140.f + index * SquareSize}, sf::Color(191, 203, 214), true);
    }

    drawGuiButton(target, font, "Undo", 40.f, ControlsTop, 110.f);
    drawGuiText(target, font, "Promotion piece", 12, {190.f, 769.f}, sf::Color(191, 203, 214));
    const PieceType choices[] = {PieceType::Queen, PieceType::Rook, PieceType::Bishop, PieceType::Knight};
    const char* labels[] = {"Queen", "Rook", "Bishop", "Knight"};
    for (int index = 0; index < 4; ++index) {
        drawGuiButton(target, font, labels[index], 190.f + index * 108.f, ControlsTop, 100.f,
                      promotionChoice == choices[index]);
    }
    drawGuiButton(target, font, "Offer draw", 40.f, DrawControlsTop, 110.f);
    drawGuiText(target, font, "Threefold and 50-move draws are offered when available.", 13,
                {190.f, 853.f}, sf::Color(191, 203, 214));
    drawGuiText(target, font, "U: undo    Esc: cancel selection    Close the window to quit", 11,
                {40.f, 892.f}, sf::Color(151, 165, 178));

    // Draw offer panel
    if (drawOffer != DrawOffer::None) {
        sf::RectangleShape shade({static_cast<float>(Width), static_cast<float>(Height)});
        shade.setFillColor(sf::Color(0, 0, 0, 170));
        target.draw(shade);

        sf::RectangleShape panel({480.f, 210.f});
        panel.setPosition({120.f, 330.f});
        panel.setFillColor(sf::Color(42, 50, 60));
        panel.setOutlineColor(sf::Color(121, 137, 150));
        panel.setOutlineThickness(2.f);
        target.draw(panel);

        std::string player = "White";
        std::string opponent = "Black";
        if (position.sideToMove == Color::Black) {
            player = "Black";
            opponent = "White";
        }
        std::string title;
        std::string explanation = player + " may claim a draw, or continue playing.";
        std::string acceptLabel = "Claim draw";
        std::string declineLabel = "Continue";
        if (drawOffer == DrawOffer::Agreement) {
            title = "Draw by agreement";
            explanation = player + " offers a draw. " + opponent + ": accept?";
            acceptLabel = "Accept";
            declineLabel = "Decline";
        } else if (drawOffer == DrawOffer::Threefold) {
            title = "Threefold repetition";
        } else {
            title = "50-move rule";
        }
        drawGuiText(target, font, title, 22, {360.f, 368.f}, sf::Color::White, true);
        drawGuiText(target, font, explanation, 16, {360.f, 413.f}, sf::Color(220, 228, 235), true);
        drawGuiButton(target, font, acceptLabel, 200.f, 450.f, 140.f);
        drawGuiButton(target, font, declineLabel, 380.f, 450.f, 140.f);
        drawGuiText(target, font, "U or Undo: go back", 12, {360.f, 516.f},
                    sf::Color(191, 203, 214), true);
    }
}

// Convert mouse clicks into moves or button actions
void Gui::handleClick(int x, int y) {
    if (x >= 40 && x < 150 && y >= ControlsTop && y < ControlsTop + 38) {
        undo();
        return;
    }

    const auto status = game.getStatus();
    if (status != GameStatus::Playing && status != GameStatus::Check) {
        message = "Game over. Undo to resume play, or close the window.";
        return;
    }

    // Answer a draw offer before accepting board input
    if (drawOffer != DrawOffer::None) {
        if (x >= 200 && x < 340 && y >= 450 && y < 488) {
            bool accepted = false;
            if (drawOffer == DrawOffer::Agreement) {
                accepted = game.agreeDraw();
            } else if (drawOffer == DrawOffer::Threefold) {
                accepted = game.claimThreefoldDraw();
            } else {
                accepted = game.claimFiftyMoveDraw();
            }
            drawOffer = DrawOffer::None;
            selectedSquare.reset();
            if (accepted) {
                message = "Draw accepted. Undo to resume play, or close the window.";
            } else {
                message = "That draw is no longer available.";
            }
        } else if (x >= 380 && x < 520 && y >= 450 && y < 488) {
            drawOffer = DrawOffer::None;
            selectedSquare.reset();
            message = "Draw declined. Game continues.";
        }
        return;
    }

    // Draw by agreement
    if (x >= 40 && x < 150 && y >= DrawControlsTop && y < DrawControlsTop + 38) {
        selectedSquare.reset();
        drawOffer = DrawOffer::Agreement;
        return;
    }

    const PieceType choices[] = {PieceType::Queen, PieceType::Rook, PieceType::Bishop, PieceType::Knight};
    for (int index = 0; index < 4; ++index) {
        const int left = 190 + index * 108;
        if (x >= left && x < left + 100 && y >= ControlsTop && y < ControlsTop + 38) {
            promotionChoice = choices[index];
            message = "Promotion piece selected. Now move your pawn to its final rank.";
            return;
        }
    }

    if (x < BoardLeft || x >= BoardLeft + 8 * SquareSize ||
        y < BoardTop || y >= BoardTop + 8 * SquareSize) {
        return;
    }
    const Square clicked{(x - BoardLeft) / SquareSize, 7 - (y - BoardTop) / SquareSize};
    const Position& position = game.getPosition();
    const Piece& piece = position.board.at(clicked.file, clicked.rank);

    if (selectedSquare && selectedSquare->file == clicked.file && selectedSquare->rank == clicked.rank) {
        clearSelection();
        return;
    }

    // Clicking another friendly piece changes the selection
    if (piece.type != PieceType::None && piece.color == position.sideToMove) {
        selectedSquare = clicked;
        message = "Choose a destination. Click the selected piece again to cancel.";
        return;
    }
    if (!selectedSquare) {
        message = "Select a piece belonging to the player whose turn it is.";
        return;
    }

    Move move{*selectedSquare, clicked};
    const Piece& movingPiece = position.board.at(move.from.file, move.from.rank);
    if (rules::canPromote(movingPiece, move.to)) {
        move.promotion = promotionChoice;
    }

    if (game.tryMove(move, message)) {
        selectedSquare.reset();
        message = "Move played. Select the next piece.";
        checkDrawOffer();
        const auto nextStatus = game.getStatus();
        if (nextStatus != GameStatus::Playing && nextStatus != GameStatus::Check) {
            message = "Game over. Undo to resume play, or close the window.";
        }
    }
}

// Restore the previous complete position
void Gui::undo() {
    selectedSquare.reset();
    drawOffer = DrawOffer::None;
    if (game.undo()) {
        message = "Undo successful.";
    } else {
        message = "No moves to undo.";
    }
    checkDrawOffer();
}

// Cancel an unfinished mouse move
void Gui::clearSelection() {
    selectedSquare.reset();
    message = "Click one of your pieces, then click its destination.";
}
