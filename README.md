# Chess

A C++ chess learning project with a terminal interface and an SFML graphical interface. Two players can play locally using the same game logic in either interface.

## Technologies

- **C++20** and the standard library for board storage, move generation, and game state.
- **SFML 3** for the graphical window, rendering, and mouse/keyboard input.
- **CMake** for building the terminal (`Chess`) and graphical (`ChessGUI`) executables.
- **CLion** as the development environment.
- Bundled **DejaVu Sans** font for interface text and **Chess.com Neo** PNG images for the pieces.

## What works so far

- Legal moves for all pieces, captures, and king safety checks.
- Castling, en passant, and promotion to queen, rook, bishop, or knight.
- Check, checkmate, and stalemate detection.
- Draw offers, threefold repetition and 50-move draw claims, plus automatic fivefold repetition and 75-move draws.
- Move history and undo, including resuming play after a result.
- A graphical board with Chess.com Neo pieces, piece selection, legal move highlights, promotion controls, and draw prompts.

SAN notation, complete dead-position detection, and a chess engine are still planned.

## Architecture and separation of concerns

Headers live in `include/`, implementations in `src/`, and graphical resources in `assets/`.

| Component | Responsibility |
| --- | --- |
| `Board` | Stores the 64 squares and pieces; provides square access and starting-board setup. |
| `Move` / `Square` | Simple data types describing coordinates, source/destination squares, and promotion choices. |
| `Position` | Stores the board, side to move, castling rights, en passant target, and move counters. `applyMove` updates this state after validation. |
| `Rules` | Generates moves, checks attacks and king safety, and compares positions for repetition. |
| `Game` | Validates moves through `Rules`, owns the current position and undo history, and determines results and draw availability. It also contains the terminal game loop. |
| `Console` | Prints the board and parses terminal move text. |
| `Gui` | Draws the board and piece sprites and translates clicks into moves and game actions. Selection and dialog state stay here. |
| `main.cpp` / `gui_main.cpp` | Start the terminal game or the SFML window and event loop. The graphical entry point loads the font and piece textures once at startup. |

Both interfaces submit moves to `Game`. `Game` checks them against legal moves from `Rules`, saves the previous position, and applies an accepted move. The interface then displays the updated board and result. Board storage and chess rules do not depend on SFML.

## Build and run

Requires a C++20 compiler and **CMake 4.3 or newer**. CMake uses an installed SFML 3 package or downloads and builds SFML 3.0.2. On Linux, SFML also needs the X11, OpenGL, FreeType, and related development packages.

```sh
cmake -S . -B build
cmake --build build
./build/ChessGUI
```

Run `./build/Chess` for the terminal interface. In CLion, select the `ChessGUI` or `Chess` target. The font and piece images are copied beside the executable automatically; the game loads them locally without an internet connection.

For a terminal-only build:

```sh
cmake -S . -B build-terminal -DCHESS_BUILD_GUI=OFF
cmake --build build-terminal
./build-terminal/Chess
```

These executable paths assume a Unix-like system and a single-configuration build generator.

## How to play

**Graphical:** click a piece, then a highlighted destination. Select a promotion piece before moving a pawn to its final rank; queen is the default. Use **Undo** or **U** to undo one move, **Esc** to clear selection, and **Offer draw** to request a draw. Available draw claims appear in a dialog.

**Terminal:** enter moves such as `e2e4`, castling as `e1g1`, or promotion as `e7e8q` (`q`, `r`, `b`, or `n` is required). Use `undo`, `draw`, or `quit`; answer draw prompts with `y` or `n`.

Draw claims are offered once the current position qualifies; claims based on an intended future move are not implemented. After a result, undo can resume play.

The `tests/` folder is kept locally and excluded from Git. CMake adds its test targets only when that folder is present and `BUILD_TESTING` is enabled.

## Piece images

The GUI uses Chess.com's **Neo** set, stored in `assets/pieces/neo/`. Each PNG represents a piece and color, such as `wp.png` for a white pawn or `bk.png` for a black king. SFML textures are loaded once and drawn as sprites sized to the board squares.

Source: [Chess.com Neo piece assets](https://www.chess.com/chess-themes/pieces/neo/150/wp.png). Piece artwork belongs to Chess.com; the bundled font's license is in `assets/fonts/LICENSE.txt`.
