#include <iostream>

#include "Game.h"

int main() {
    Game game;
    if (game.chooseMode(std::cin, std::cout)) {
        game.run(std::cin, std::cout);
    }
    return 0;
}
