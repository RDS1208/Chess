#include "../include/Game.h"
#include "../include/Console.h"
#include "../include/Rules.h"

#include <iostream>
#include <sstream>

int main() {
    int failures = 0;
    auto check = [&failures](bool condition, const char* message) {
        if (!condition) {
            std::cerr << message << '\n';
            ++failures;
        }
    };
    std::string error;

    Position position;
    position.board.setPiece(4, 0, {PieceType::King, Color::White});
    position.board.setPiece(4, 7, {PieceType::King, Color::Black});
    position.board.setPiece(0, 0, {PieceType::Rook, Color::White});
    position.board.setPiece(0, 7, {PieceType::Rook, Color::Black});

    // Fifty moves by each side means 100 halfmoves.
    position.halfmoveClock = 99;
    Game fifty(position);
    check(!fifty.claimFiftyMoveDraw(), "Refuse a current-position claim at 99 halfmoves");
    check(fifty.tryMove(*parseMove("a1b1"), error) &&
          fifty.getPosition().halfmoveClock == 100 && fifty.getStatus() == GameStatus::Playing,
          "Reaching 100 halfmoves makes a claim available without ending play");
    check(fifty.claimFiftyMoveDraw() && fifty.getStatus() == GameStatus::DrawByFiftyMoveRule &&
          !fifty.tryMove(*parseMove("a8b8"), error),
          "An accepted 50-move claim blocks further moves");
    check(fifty.undo() && fifty.getPosition().halfmoveClock == 99 &&
          fifty.getStatus() == GameStatus::Playing,
          "Undo restores the counter and clears the 50-move result");

    Game capture(position);
    check(capture.tryMove(*parseMove("a1a8"), error) &&
          capture.getPosition().halfmoveClock == 0 && !capture.claimFiftyMoveDraw(),
          "A capture resets the counter instead of allowing a claim");

    Position pawnPosition = position;
    pawnPosition.board.setPiece(1, 1, {PieceType::Pawn, Color::White});
    Game pawn(pawnPosition);
    check(pawn.tryMove(*parseMove("b2b3"), error) &&
          pawn.getPosition().halfmoveClock == 0 && !pawn.claimFiftyMoveDraw(),
          "A pawn move resets the counter instead of allowing a claim");

    // The 75-move rule is automatic, but a mating move wins.
    position.halfmoveClock = 149;
    Game automatic(position);
    check(automatic.getStatus() == GameStatus::Playing &&
          automatic.tryMove(*parseMove("a1b1"), error) &&
          automatic.getStatus() == GameStatus::DrawBySeventyFiveMoveRule &&
          !automatic.tryMove(*parseMove("a8b8"), error),
          "The 75-move draw begins at 150 halfmoves and blocks moves");
    check(automatic.undo() && automatic.getStatus() == GameStatus::Playing &&
          automatic.getPosition().halfmoveClock == 149,
          "Undo resumes play after an automatic 75-move draw");

    Position mating;
    mating.board.setPiece(5, 5, {PieceType::King, Color::White});
    mating.board.setPiece(6, 5, {PieceType::Queen, Color::White});
    mating.board.setPiece(7, 7, {PieceType::King, Color::Black});
    mating.halfmoveClock = 149;
    Game mate(mating);
    check(mate.tryMove(*parseMove("g6g7"), error) &&
          mate.getPosition().halfmoveClock == 150 && mate.getStatus() == GameStatus::Checkmate,
          "Checkmate takes precedence on the move reaching 150 halfmoves");
    check(!mate.claimFiftyMoveDraw(), "A completed game cannot be replaced by a claim");

    // Four returns to the starting position make five occurrences in total.
    Game fivefold;
    for (int cycle = 0; cycle < 4; ++cycle) {
        for (const char* move : {"g1f3", "g8f6", "f3g1", "f6g8"}) {
            check(fivefold.tryMove(*parseMove(move), error), "Play the repetition fixture");
        }
        if (cycle < 3) {
            check(fivefold.getStatus() == GameStatus::Playing,
                  "Three or four occurrences do not automatically end play");
        }
    }
    check(fivefold.getStatus() == GameStatus::DrawByFivefoldRepetition &&
          !fivefold.tryMove(*parseMove("e2e4"), error),
          "The fifth occurrence automatically ends play");
    Game fivefoldConsole = fivefold;
    std::istringstream afterFivefold("e2e4\nundo\nn\nb8c6\nquit\n");
    std::ostringstream fivefoldOutput;
    fivefoldConsole.run(afterFivefold, fivefoldOutput);
    check(fivefoldOutput.str().find("Draw by fivefold repetition") != std::string::npos &&
          fivefoldOutput.str().find("Only undo or quit") != std::string::npos &&
          fivefoldConsole.getStatus() == GameStatus::Playing &&
          fivefoldConsole.getPosition().board.at(2, 5).type == PieceType::Knight,
          "An automatic repetition result is announced and console undo resumes play");
    check(fivefold.undo() && fivefold.getStatus() == GameStatus::Playing &&
          fivefold.tryMove(*parseMove("b8c6"), error),
          "Undo removes the fifth occurrence and allows a different reply");

    // The console offers a draw only after the current position qualifies.
    position.halfmoveClock = 100;
    Game console(position);
    std::istringstream accept("maybe\nY\na1b1\nquit\n");
    std::ostringstream output;
    console.run(accept, output);
    check(console.getStatus() == GameStatus::DrawByFiftyMoveRule &&
          output.str().find("Please enter y or n") != std::string::npos &&
          output.str().find("Only undo or quit") != std::string::npos,
          "The console retries invalid answers and restricts play after a 50-move claim");

    Game declined(position);
    std::istringstream decline("n\na1b1\nn\nquit\n");
    std::ostringstream declineOutput;
    declined.run(decline, declineOutput);
    check(declined.getStatus() == GameStatus::Playing &&
          declined.getPosition().board.at(1, 0).type == PieceType::Rook,
          "Declining the 50-move claim continues play");

    position.halfmoveClock = 99;
    Game removedCommands(position);
    std::istringstream oldCommands("claim3\nclaim50\nclaim3 a1b1\nclaim50 a1b1\nquit\n");
    std::ostringstream oldOutput;
    removedCommands.run(oldCommands, oldOutput);
    check(removedCommands.getStatus() == GameStatus::Playing &&
          removedCommands.getPosition().halfmoveClock == 99 && !removedCommands.undo() &&
          oldOutput.str().find("Invalid input") != std::string::npos &&
          oldOutput.str().find("Draw claimed") == std::string::npos,
          "Removed claim commands cannot end the game or change the position");

    position.halfmoveClock = 100;
    Game initialClaim(position);
    check(initialClaim.claimFiftyMoveDraw() && initialClaim.undo() &&
          initialClaim.getStatus() == GameStatus::Playing && !initialClaim.undo(),
          "Undo clears a claim made before any moves without creating history");
    Game unanswered(position);
    std::istringstream endOfInput("");
    std::ostringstream endOutput;
    unanswered.run(endOfInput, endOutput);
    check(unanswered.getStatus() == GameStatus::Playing,
          "End-of-file at the 50-move prompt does not claim a draw");

    Position checking;
    checking.board.setPiece(0, 0, {PieceType::King, Color::White});
    checking.board.setPiece(4, 0, {PieceType::Rook, Color::White});
    checking.board.setPiece(4, 7, {PieceType::King, Color::Black});
    checking.sideToMove = Color::Black;
    checking.halfmoveClock = 150;
    Game checked(checking);
    check(checked.getStatus() == GameStatus::DrawBySeventyFiveMoveRule,
          "Check without checkmate does not prevent an automatic 75-move draw");

    if (failures != 0) {
        return 1;
    }
    std::cout << "All draw checks passed.\n";
}
