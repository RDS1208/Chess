#include "Gui.h"

#include <iostream>
#include <filesystem>

int main() {
    sf::RenderWindow window(sf::VideoMode({Gui::Width, Gui::Height}), "Chess",
                            sf::Style::Titlebar | sf::Style::Close);
    window.setFramerateLimit(60);

    sf::Font font;
    if (!font.openFromFile(CHESS_FONT_PATH)) {
        std::cerr << "Could not load the chess font. Rebuild the ChessGUI target to copy assets.\n";
        return 1;
    }

    Gui::PieceTextures pieces;
    const char* filenames[] = {
        "wp.png", "wn.png", "wb.png", "wr.png", "wq.png", "wk.png",
        "bp.png", "bn.png", "bb.png", "br.png", "bq.png", "bk.png"
    };
    for (std::size_t index = 0; index < pieces.size(); ++index) {
        const auto path = std::filesystem::path(CHESS_PIECES_PATH) / filenames[index];
        if (!pieces[index].loadFromFile(path)) {
            std::cerr << "Could not load chess piece: " << path
                      << ". Rebuild the ChessGUI target to copy assets.\n";
            return 1;
        }
        pieces[index].setSmooth(true);
    }

    Game game;
    Gui gui(game);

    while (window.isOpen()) {
        // Mouse and keyboard input
        while (const auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            } else if (const auto* click = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (click->button == sf::Mouse::Button::Left) {
                    gui.handleClick(click->position.x, click->position.y);
                }
            } else if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
                if (key->code == sf::Keyboard::Key::U) {
                    gui.undo();
                } else if (key->code == sf::Keyboard::Key::Escape) {
                    gui.clearSelection();
                }
            }
        }

        // Redraw the interface
        if (window.isOpen()) {
            window.clear(sf::Color(31, 37, 45));
            gui.draw(window, font, pieces);
            window.display();
        }
    }
    return 0;
}
