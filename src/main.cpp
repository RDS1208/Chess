#include <iostream>
#include <string>

#include "Console.h"
#include "Position.h"

int main() {
    Position position = createStartingPosition();
    printBoard(position.board, std::cout);

    std::string text;
    while (true) {
        std::cout << "Enter a move (e.g. e2e4), or quit: ";
        if (!std::getline(std::cin, text)) {
            std::cout << '\n';
            break;
        }

        if (text == "quit") {
            std::cout << "Exiting game.\n";
            break;
        }

        const auto move = parseMove(text);
        if (!move) {
            std::cout << "Invalid input. Use a move such as e2e4.\n";
            continue;
        }

        std::cout << "Parsed move: (" << move->from.file << ", "
                  << move->from.rank << ") -> (" << move->to.file << ", "
                  << move->to.rank << ")\n";
    }

    return 0;
}
