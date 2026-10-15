# Chess

A C++20 chess project built in CLion as a learning project. Development starts with a terminal chess game, followed by a graphical interface and a chess engine.

## Current progress

The board representation is implemented:

- Strongly typed enums for piece types and colors.
- A `Piece` struct containing its type and color.
- A `Board` class storing 64 squares in `std::array<Piece, 64>`.
- Read-only square access with coordinate validation.
- Clearing the board and setting up the standard starting position.

The executable displays the starting board and parses coordinate input such as `e2e4`. It rejects malformed input, prints the parsed coordinates, and exits on `quit` or end-of-file. Parsing does not move pieces or check move legality yet. Legal moves, check, checkmate, special moves, graphics, and an engine are planned.

The rules module generates ordinary pseudo-legal moves for all six piece types, including pawn diagonal captures and initial double advances. It checks turns, boundaries, blockers, friendly occupancy, and king-capture exclusion. Both sides have 20 moves from the starting position. King-safety filtering, promotion, en passant, and castling are still pending. `Board::setPiece()` supports bounds-checked position setup.

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
| `include/Position.h`, `src/Position.cpp` | Complete position state and starting-position construction |
| `include/Rules.h`, `src/Rules.cpp` | Ordinary pseudo-legal move generation |
| `include/Console.h`, `src/Console.cpp` | Board display and move input parsing |
| `src/main.cpp` | Current executable entry point |
| `CMakeLists.txt` | C++20 executable configuration |
| `tests/MoveTests.cpp` | Move parsing checks |
| `tests/KnightMovesTests.cpp` | Knight movement, captures, boundaries, and board setup checks |
| `tests/PseudoLegalMovesTests.cpp` | Opening counts, pawn movement/captures, and king-capture rejection |

## Build and run

Requirements: a C++20 compiler and CMake 4.3 or newer, matching the current project configuration.

Open the project in CLion, let CMake configure it, and build/run the `Chess` target. Alternatively, from the project directory:

```sh
cmake -S . -B build
cmake --build build
./build/Chess
```

Run the parsing checks after building:

```sh
ctest --test-dir build --output-on-failure
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
