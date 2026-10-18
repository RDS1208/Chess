# Chess

A C++20 chess project built in CLion as a learning project. Development starts with a terminal chess game, followed by a graphical interface and a chess engine.

## Current progress

The board representation is implemented:

- Strongly typed enums for piece types and colors.
- A `Piece` struct containing its type and color.
- A `Board` class storing 64 squares in `std::array<Piece, 64>`.
- Read-only square access with coordinate validation.
- Clearing the board and setting up the standard starting position.

The project has a simple SFML graphical interface (`ChessGUI`) and a terminal interface (`Chess`). Both use the same `Game` class and rules, including check, checkmate, stalemate, promotion, en passant, and castling.

In the graphical interface, click one of your pieces and then a destination. Legal destinations are highlighted. Click Undo (or press U) to reverse one player's move; Esc cancels a selection. Choose Queen, Rook, Bishop, or Knight before promoting; Queen is selected initially. Turn, errors, check, and results appear above the board. Threefold repetition and the 50-move rule open a panel with **Claim draw** and **Continue** buttons. Click **Offer draw** for an agreement, then the other player chooses **Accept** or **Decline**. A pending offer blocks board input; Undo remains available. Declining dismisses the offer for the current position; the next accepted move or undo checks the new position again. Fivefold repetition and the 75-move rule end play automatically. After any result, undo resumes play; close the window to quit.

Use moves such as `e2e4`, castling moves such as `e1g1`, and promotion moves such as `e7e8n` (q, r, b, or n is required). Enter `draw` to offer a draw; the other player answers `y` or `n`. Enter `undo` to reverse one player's move, or `quit` to exit. End-of-file also exits cleanly. After any result, only `undo` or `quit` is accepted. Undo clears a claimed/agreed result and restores the previous position when available, allowing play to resume.

The terminal automatically checks the current position for threefold repetition and the 50-move rule (100 halfmoves without a pawn move or capture). When eligible, it offers a draw: answer `y` to accept or `n` to continue. This project uses offers after the condition occurs; claims based on future moves are omitted by choice. Fivefold repetition and 150 halfmoves (75 moves by each player) end the game automatically. Checkmate on the final move takes precedence. See [FIDE Articles 9.2, 9.3, and 9.6](https://handbook.fide.com/chapter/E012023) for the official draw rules.

Dead-position detection remains for the project's author to implement, along with SAN notation and an engine. Twelve CTest suites cover parsing, move generation, special moves, state updates, draws, history, console behavior, and graphical input. `Game` can also start from a supplied `Position`; history begins empty for that position.

## Board coordinates

Files `a` through `h` map to indices `0` through `7`. Ranks `1` through `8` also map to indices `0` through `7`.

The array index is `rank * 8 + file`: `a1` is index `0`, and `h8` is index `63`. Empty squares have type `PieceType::None`; their color is ignored.

## Board API

```cpp
#include "Board.h"

Board board;                    // Starts empty.
board.resetStartingPosition();  // Sets up all 32 starting pieces.
const Piece& piece = board.at(4, 1); // e2: a white pawn.
board.clear();                  // Empties all squares.
```

`at(file, rank)` throws `std::out_of_range` if either coordinate is outside `0` through `7`.

## Project files

Production headers are in `include/`, implementation files are in `src/`, and test sources are in `tests/`. CMake adds `include/` to the header search path, so source files can use includes such as `#include "Board.h"`.

| File | Purpose |
| --- | --- |
| `include/Board.h` | Piece types, colors, piece data, and the board interface |
| `src/Board.cpp` | Square access, clearing, and starting-position setup |
| `include/Move.h` | Square coordinates and move data |
| `include/Position.h`, `src/Position.cpp` | Complete position state, initialization, and move application |
| `include/Rules.h`, `src/Rules.cpp` | Pseudo-legal/legal moves, special moves, attack and check detection |
| `include/Console.h`, `src/Console.cpp` | Board display and move input parsing |
| `include/Game.h`, `src/Game.cpp` | Terminal loop, move validation, results, and undo history |
| `include/Gui.h`, `src/Gui.cpp` | SFML board drawing, click selection, promotion controls, draw offers, and undo |
| `src/gui_main.cpp` | Graphical window and mouse/keyboard event loop |
| `assets/fonts/` | Bundled DejaVu Sans font and its license |
| `src/main.cpp` | Starts the terminal game |
| `CMakeLists.txt` | C++20 executable configuration |
| `tests/MoveTests.cpp` | Move parsing checks |
| `tests/KnightMovesTests.cpp` | Knight movement, captures, boundaries, and board setup checks |
| `tests/PseudoLegalMovesTests.cpp` | Opening counts, pawn movement/captures, and king-capture rejection |
| `tests/PositionTests.cpp`, `tests/AttackTests.cpp`, `tests/CheckTests.cpp` | State updates, attacks, checks, and basic legal moves |
| `tests/PromotionTests.cpp`, `tests/EnPassantTests.cpp`, `tests/CastlingTests.cpp` | Special-move checks |
| `tests/GameTests.cpp` | Accepted/rejected moves, history, undo, and console behavior |
| `tests/DrawTests.cpp` | Draw thresholds, acceptance prompts, automatic results, and undo |
| `tests/GuiTests.cpp` | Mouse coordinates, move rejection, selection, promotion, draw prompts, automatic results, and undo |

## Build and run

Requirements: a C++20 compiler and CMake 4.3 or newer, matching the current project configuration.

Open the project in CLion, reload CMake, and select **ChessGUI** to play graphically, or **Chess** for the terminal. Alternatively, from the project directory:

```sh
cmake -S . -B build
cmake --build build
./build/ChessGUI
```

Run `./build/Chess` for the terminal version. CMake uses an installed SFML 3 package, or downloads SFML 3.0.2 and builds the graphics modules. The font is copied beside the graphical executable automatically. This setup follows the [official SFML CMake approach](https://www.sfml-dev.org/tutorials/3.0/getting-started/cmake/).

On Linux, building SFML requires the window, OpenGL, and FreeType development packages. For Ubuntu:

```sh
sudo apt install libx11-dev libxrandr-dev libxcursor-dev libxi-dev libudev-dev libfreetype-dev libgl-dev
```

For a terminal-only build without SFML or its download:

```sh
cmake -S . -B build-terminal -DCHESS_BUILD_GUI=OFF
cmake --build build-terminal
```

Run the tests after building:

```sh
ctest --test-dir build --output-on-failure
```

The run command above assumes a Unix-like system and a single-configuration build generator.

## Planned milestones

1. Add SAN display and input.
2. Complete draw rules and broaden move-generation verification.
3. Build and improve a chess engine.

## Local files

Build artifacts, IDE settings, local environment files, and Markdown guides other than this README are excluded by `.gitignore`.
