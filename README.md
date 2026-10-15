# Chess

A C++20 chess project built in CLion as a learning project. Development starts with a terminal chess game, followed by a graphical interface and a chess engine.

## Current progress

The board representation is implemented:

- Strongly typed enums for piece types and colors.
- A `Piece` struct containing its type and color.
- A `Board` class storing 64 squares in `std::array<Piece, 64>`.
- Read-only square access with coordinate validation.
- Clearing the board and setting up the standard starting position.

The executable currently runs the CLion starter demonstration. It does not display the board or accept chess moves yet. Legal moves, check, checkmate, special moves, graphics, and an engine are planned.

## Board coordinates

Files `a` through `h` map to indices `0` through `7`. Ranks `1` through `8` also map to indices `0` through `7`.

The array index is `rank * 8 + file`: `a1` is index `0`, and `h8` is index `63`. Empty squares have type `PieceType::None`; their color is ignored.

## Board API

```cpp
#include "Board.h"

Board board;                    // Starts empty.
board.ResetStartingPosition();  // Sets up all 32 starting pieces.
const Piece& piece = board.at(4, 1); // e2: a white pawn.
board.clear();                  // Empties all squares.
```

`at(file, rank)` throws `std::out_of_range` if either coordinate is outside `0` through `7`.

## Project files

| File | Purpose |
| --- | --- |
| `Board.h` | Piece types, colors, piece data, and the board interface |
| `Board.cpp` | Square access, clearing, and starting-position setup |
| `main.cpp` | Current executable entry point |
| `CMakeLists.txt` | C++20 executable configuration |

## Build and run

Requirements: a C++20 compiler and CMake 4.3 or newer, matching the current project configuration.

Open the project in CLion, let CMake configure it, and build/run the `Chess` target. Alternatively, from the project directory:

```sh
cmake -S . -B build
cmake --build build
./build/Chess
```

The run command above assumes a Unix-like system and a single-configuration build generator.

## Planned milestones

1. Print the board and parse terminal input.
2. Implement piece movement, turns, and captures.
3. Enforce king safety and detect check, checkmate, and stalemate.
4. Implement promotion, en passant, castling, and draw rules.
5. Verify move generation and complete the terminal game.
6. Add a graphical interface.
7. Build and improve a chess engine.

## Local files

Build artifacts, IDE settings, local environment files, and Markdown guides other than this README are excluded by `.gitignore`.
