#include "../include/Gui.h"
#include "../include/Console.h"

#include <iostream>

int main() {
    int failures = 0;
    auto check = [&failures](bool condition, const char* name) {
        if (!condition) {
            std::cerr << name << '\n';
            ++failures;
        }
    };

    Game game;
    Gui gui(game);
    gui.handleClick(400, 220); // Black pawn on e7: White cannot select it.
    gui.handleClick(400, 380); // e5
    check(game.getPosition().sideToMove == Color::White,
          "Clicking opponent's pieces does not play a move");

    gui.handleClick(400, 620); // Select e2.
    gui.handleClick(400, 380); // e2e5 is illegal.
    check(game.getPosition().board.at(4, 1).type == PieceType::Pawn &&
          game.getPosition().sideToMove == Color::White,
          "Illegal click move preserves board and turn");
    gui.handleClick(400, 460); // e2e4
    check(game.getPosition().board.at(4, 3).type == PieceType::Pawn &&
          game.getPosition().board.at(4, 1).type == PieceType::None &&
          game.getPosition().sideToMove == Color::Black,
          "Click coordinates map e2e4 correctly with White at the bottom");

    gui.handleClick(400, 220); // e7
    gui.handleClick(400, 380); // e5
    check(game.getPosition().board.at(4, 4).color == Color::Black &&
          game.getPosition().board.at(4, 4).type == PieceType::Pawn &&
          game.getPosition().sideToMove == Color::White,
          "Black can reply through the same board");
    gui.handleClick(90, 809); // Undo button.
    check(game.getPosition().sideToMove == Color::Black &&
          game.getPosition().board.at(4, 6).type == PieceType::Pawn,
          "Undo button restores previous board and turn");
    gui.undo();
    gui.undo(); // Empty history is harmless.
    check(game.getPosition().sideToMove == Color::White &&
          game.getPosition().board.at(4, 1).type == PieceType::Pawn,
          "Undo works down to the initial position");

    gui.handleClick(400, 620);
    gui.clearSelection();
    gui.handleClick(400, 460);
    check(game.getPosition().sideToMove == Color::White,
          "Clearing selection cancels an unfinished move");
    gui.handleClick(-1, 100);
    gui.handleClick(720, 740);
    check(game.getPosition().sideToMove == Color::White,
          "Clicks outside board and buttons do nothing");

    // Choosing another friendly piece changes the selection.
    gui.handleClick(400, 620); // e2
    gui.handleClick(160, 700); // b1
    gui.handleClick(240, 540); // c3
    check(game.getPosition().board.at(2, 2).type == PieceType::Knight &&
          game.getPosition().board.at(4, 1).type == PieceType::Pawn,
          "Friendly-piece clicks change selection instead of capturing");

    const PieceType choices[] = {
        PieceType::Queen, PieceType::Rook, PieceType::Bishop, PieceType::Knight
    };
    for (int index = 0; index < 4; ++index) {
        Game promotion;
        std::string error;
        for (const char* move : {"a2a4", "h7h5", "a4a5", "h5h4",
                                 "a5a6", "h4h3", "a6b7", "h3g2"}) {
            check(promotion.tryMove(*parseMove(move), error), "Set up a promotion");
        }
        Gui promotionGui(promotion);
        promotionGui.handleClick(240 + index * 108, 809); // Promotion-choice button.
        promotionGui.handleClick(160, 220); // b7
        promotionGui.handleClick(80, 140); // a8
        check(promotion.getPosition().board.at(0, 7).type == choices[index] &&
              promotion.getPosition().board.at(0, 7).color == Color::White,
              "Promotion buttons select the piece before moving the pawn");
    }

    Game mate;
    std::string error;
    for (const char* move : {"f2f3", "e7e5", "g2g4", "d8h4"}) {
        check(mate.tryMove(*parseMove(move), error), "Set up checkmate");
    }
    Gui mateGui(mate);
    mateGui.handleClick(80, 620); // a2
    mateGui.handleClick(80, 540); // a3
    check(mate.getStatus() == GameStatus::Checkmate &&
          mate.getPosition().board.at(0, 1).type == PieceType::Pawn,
          "Board clicks cannot play moves after checkmate");
    mateGui.undo();
    check(mate.getStatus() == GameStatus::Playing,
          "Undo after checkmate resumes play");

    Game drawnGame;
    Gui drawnGui(drawnGame);
    check(drawnGame.agreeDraw(), "Record an agreement in the shared game");
    drawnGui.handleClick(400, 620); // e2
    drawnGui.handleClick(400, 460); // e4
    check(drawnGame.getStatus() == GameStatus::DrawByAgreement &&
          drawnGame.getPosition().board.at(4, 1).type == PieceType::Pawn,
          "An agreed draw blocks graphical moves too");
    drawnGui.undo();
    drawnGui.handleClick(400, 620);
    drawnGui.handleClick(400, 460);
    check(drawnGame.getStatus() == GameStatus::Playing &&
          drawnGame.getPosition().board.at(4, 3).type == PieceType::Pawn,
          "Graphical undo clears an agreement and resumes play");

    Position quiet;
    quiet.board.setPiece(4, 0, {PieceType::King, Color::White});
    quiet.board.setPiece(4, 7, {PieceType::King, Color::Black});
    quiet.board.setPiece(0, 0, {PieceType::Rook, Color::White});
    quiet.halfmoveClock = 149;
    Game seventyFive(quiet);
    Gui seventyFiveGui(seventyFive);
    seventyFiveGui.handleClick(450, 469); // Continue past the available 50-move draw.
    seventyFiveGui.handleClick(80, 700); // a1
    seventyFiveGui.handleClick(160, 700); // b1
    check(seventyFive.getStatus() == GameStatus::DrawBySeventyFiveMoveRule,
          "Graphical moves trigger an automatic 75-move draw");
    seventyFiveGui.handleClick(400, 140); // e8
    seventyFiveGui.handleClick(480, 140); // f8
    check(seventyFive.getPosition().board.at(4, 7).type == PieceType::King,
          "An automatic draw blocks further graphical moves");
    seventyFiveGui.undo();
    check(seventyFive.getStatus() == GameStatus::Playing &&
          seventyFive.getPosition().halfmoveClock == 149,
          "Graphical undo resumes play after an automatic draw");

    // A qualifying position opens a draw prompt and blocks the board until answered.
    quiet.halfmoveClock = 99;
    Game fifty(quiet);
    Gui fiftyGui(fifty);
    fiftyGui.handleClick(80, 700); // a1
    fiftyGui.handleClick(160, 700); // b1: the counter reaches 100.
    fiftyGui.handleClick(400, 140); // e8
    fiftyGui.handleClick(480, 140); // f8: blocked by the prompt.
    check(fifty.getPosition().board.at(4, 7).type == PieceType::King &&
          fifty.getPosition().halfmoveClock == 100,
          "The draw prompt blocks board moves until answered");
    fiftyGui.handleClick(270, 469); // Claim draw.
    check(fifty.getStatus() == GameStatus::DrawByFiftyMoveRule,
          "Claiming the graphical 50-move offer records the result");
    fiftyGui.undo();
    check(fifty.getStatus() == GameStatus::Playing && fifty.getPosition().halfmoveClock == 99,
          "Undo clears a graphical claim and restores the previous counter");
    fiftyGui.handleClick(80, 700);
    fiftyGui.handleClick(160, 700);
    fiftyGui.handleClick(450, 469); // Continue.
    fiftyGui.clearSelection(); // Cancelling a selection must not reopen the draw prompt.
    fiftyGui.handleClick(400, 140);
    fiftyGui.handleClick(480, 140);
    check(fifty.getPosition().board.at(5, 7).type == PieceType::King &&
          fifty.getPosition().halfmoveClock == 101,
          "Declining a graphical draw allows the next move");
    fiftyGui.handleClick(270, 469);
    check(fifty.getStatus() == GameStatus::DrawByFiftyMoveRule,
          "The next qualifying position offers a draw again");

    const int knightMoves[][4] = {
        {560, 700, 480, 540}, // g1f3
        {560, 140, 480, 300}, // g8f6
        {480, 540, 560, 700}, // f3g1
        {480, 300, 560, 140}  // f6g8
    };
    Game repetition;
    Gui repetitionGui(repetition);
    for (int cycle = 0; cycle < 2; ++cycle) {
        for (const auto& move : knightMoves) {
            repetitionGui.handleClick(move[0], move[1]);
            repetitionGui.handleClick(move[2], move[3]);
        }
    }
    repetitionGui.handleClick(270, 469);
    check(repetition.getStatus() == GameStatus::DrawByRepetition,
          "The third occurrence offers a graphical repetition claim");
    repetitionGui.undo();
    check(repetition.getStatus() == GameStatus::Playing &&
          repetition.getPosition().sideToMove == Color::Black,
          "Undo resumes play after a graphical repetition claim");

    Game fivefold;
    Gui fivefoldGui(fivefold);
    int movesPlayed = 0;
    for (int cycle = 0; cycle < 4; ++cycle) {
        for (const auto& move : knightMoves) {
            fivefoldGui.handleClick(move[0], move[1]);
            fivefoldGui.handleClick(move[2], move[3]);
            ++movesPlayed;
            if (movesPlayed >= 8 && movesPlayed < 16) {
                fivefoldGui.handleClick(450, 469); // Decline each available claim.
            }
        }
    }
    check(fivefold.getStatus() == GameStatus::DrawByFivefoldRepetition,
          "Declining repetition claims still leads to an automatic fivefold draw");
    fivefoldGui.handleClick(400, 620);
    fivefoldGui.handleClick(400, 460);
    check(fivefold.getPosition().board.at(4, 1).type == PieceType::Pawn,
          "An automatic fivefold draw blocks graphical moves");
    fivefoldGui.undo();
    fivefoldGui.handleClick(450, 469); // The restored position still permits a threefold claim.
    fivefoldGui.handleClick(160, 140); // b8
    fivefoldGui.handleClick(240, 300); // c6
    check(fivefold.getStatus() == GameStatus::Playing &&
          fivefold.getPosition().board.at(2, 5).type == PieceType::Knight,
          "Undo after fivefold refreshes the offer and allows a different move");

    Game agreement;
    Gui agreementGui(agreement);
    agreementGui.handleClick(90, 861); // Offer draw.
    agreementGui.handleClick(450, 469); // Decline.
    agreementGui.handleClick(400, 620);
    agreementGui.handleClick(400, 460);
    check(agreement.getPosition().board.at(4, 3).type == PieceType::Pawn,
          "Declining an agreement offer allows play to continue");
    agreementGui.handleClick(90, 861);
    agreementGui.handleClick(270, 469); // Accept.
    check(agreement.getStatus() == GameStatus::DrawByAgreement,
          "The other player can accept an agreement through the graphical prompt");
    agreementGui.undo();
    agreementGui.handleClick(90, 861);
    agreementGui.undo(); // Undo also works while an offer is pending.
    check(agreement.getStatus() == GameStatus::Playing &&
          agreement.getPosition().board.at(4, 1).type == PieceType::Pawn,
          "Undo dismisses a pending draw offer and restores the previous position");
    agreementGui.handleClick(400, 620);
    agreementGui.handleClick(400, 460);
    check(agreement.getPosition().board.at(4, 3).type == PieceType::Pawn,
          "Undo with empty history also dismisses an agreement offer");

    if (failures != 0) {
        return 1;
    }
    std::cout << "All graphical input checks passed.\n";
}
