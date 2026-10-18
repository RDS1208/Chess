#ifndef CHESS_GUI_H
#define CHESS_GUI_H

#include "Game.h"

#include <SFML/Graphics.hpp>
#include <optional>
#include <string>

class Gui {
public:
    static constexpr int Width = 720;
    static constexpr int Height = 910;

    explicit Gui(Game& game);

    void draw(sf::RenderTarget& target, const sf::Font& font) const;
    void handleClick(int x, int y);
    void undo();
    void clearSelection();

private:
    enum class DrawOffer { None, Agreement, Threefold, FiftyMove };

    void checkDrawOffer();

    static constexpr int BoardLeft = 40;
    static constexpr int BoardTop = 100;
    static constexpr int SquareSize = 80;
    static constexpr int ControlsTop = 790;
    static constexpr int DrawControlsTop = 842;

    Game& game;
    std::optional<Square> selectedSquare;
    PieceType promotionChoice = PieceType::Queen;
    DrawOffer drawOffer = DrawOffer::None;
    std::string message = "Click one of your pieces, then click its destination.";
};

#endif // CHESS_GUI_H
