#include "../include/Game.h"
#include "../include/Console.h"
#include "../include/Rules.h"

#include <iostream>
#include <sstream>

bool samePosition(const Position& first, const Position& second) {
    for (int rank = 0; rank < 8; ++rank) {
        for (int file = 0; file < 8; ++file) {
            const Piece& a = first.board.at(file, rank);
            const Piece& b = second.board.at(file, rank);
            if (a.type != b.type || a.color != b.color) {
                return false;
            }
        }
    }
    if (first.enPassantTarget.has_value() != second.enPassantTarget.has_value()) {
        return false;
    }
    if (first.enPassantTarget &&
        (first.enPassantTarget->file != second.enPassantTarget->file ||
         first.enPassantTarget->rank != second.enPassantTarget->rank)) {
        return false;
    }
    return first.sideToMove == second.sideToMove &&
           first.whiteCanCastleKingside == second.whiteCanCastleKingside &&
           first.whiteCanCastleQueenside == second.whiteCanCastleQueenside &&
           first.blackCanCastleKingside == second.blackCanCastleKingside &&
           first.blackCanCastleQueenside == second.blackCanCastleQueenside &&
           first.halfmoveClock == second.halfmoveClock &&
           first.fullmoveNumber == second.fullmoveNumber;
}

int main() {
    int failures = 0;
    auto check = [&failures](bool condition, const char* name) {
        if (!condition) {
            std::cerr << name << '\n';
            ++failures;
        }
    };
    std::string error;

    // Rejected moves must preserve the position and leave history empty.
    for (const Move move : {
        Move{{4, 2}, {4, 3}},                 // Empty source
        Move{{4, 6}, {4, 4}},                 // Opponent's pawn
        Move{{4, 1}, {4, 4}},                 // Pawn moving three squares
        Move{{0, 0}, {0, 2}},                 // Blocked rook
        Move{{3, 0}, {4, 0}},                 // Friendly destination
        Move{{4, 1}, {4, 1}},                 // Same square
        Move{{-1, 1}, {4, 3}},                // Invalid source
        Move{{4, 1}, {4, 8}},                 // Invalid destination
        Move{{4, 1}, {4, 3}, PieceType::Queen} // Promotion away from final rank
    }) {
        Game game;
        const Position before = game.getPosition();
        error.clear();
        check(!game.tryMove(move, error) && !error.empty(),
              "Reject illegal move and explain why");
        check(samePosition(before, game.getPosition()), "Rejected move preserves the full position");
        check(!game.undo(), "Rejected move does not create a history entry");
    }

    // Accepted moves save the previous complete position for undo.
    Game opening;
    const Position start = opening.getPosition();
    check(!opening.claimThreefoldDraw() && opening.getStatus() == GameStatus::Playing,
          "A claim before any repetition is refused without ending play");
    error = "Previous error";
    check(opening.tryMove(*parseMove("e2e4"), error) && error.empty(),
          "Accept e2e4 and clear old error");
    const Position afterWhite = opening.getPosition();
    check(afterWhite.sideToMove == Color::Black &&
          afterWhite.board.at(4, 3).type == PieceType::Pawn,
          "Accepted move updates board and turn");
    check(opening.tryMove(*parseMove("e7e5"), error), "Accept Black's reply");
    const Position afterBlack = opening.getPosition();
    check(!opening.tryMove(*parseMove("e4e6"), error), "Reject invalid move during play");
    check(samePosition(afterBlack, opening.getPosition()), "Rejected move preserves ongoing game");
    check(opening.undo() && samePosition(afterWhite, opening.getPosition()),
          "Undo skips rejected move and restores the previous accepted position");
    check(opening.undo() && samePosition(start, opening.getPosition()) && !opening.undo(),
          "History behaves like a stack and ends at the starting position");

    // A knight pinned by a bishop cannot expose its king.
    Game pinned;
    for (const char* text : {"e2e4", "d7d6", "f1b5", "b8c6", "h2h3"}) {
        check(pinned.tryMove(*parseMove(text), error), "Set up pinned knight with legal moves");
    }
    const Position beforePinnedMove = pinned.getPosition();
    check(!pinned.tryMove(*parseMove("c6b4"), error) && error.find("check") != std::string::npos,
          "Explain rejection of a move exposing the king");
    check(samePosition(beforePinnedMove, pinned.getPosition()), "Pinned move leaves position intact");

    // Promotion requires the fifth character and preserves history on rejection.
    Game promotion;
    for (const char* text : {"a2a4", "h7h5", "a4a5", "h5h4",
                             "a5a6", "h4h3", "a6b7", "h3g2"}) {
        check(promotion.tryMove(*parseMove(text), error), "Set up promotion with legal moves");
    }
    const Position beforePromotion = promotion.getPosition();
    check(!promotion.tryMove(*parseMove("b7a8"), error) && !error.empty(),
          "Reject missing promotion choice");
    check(samePosition(beforePromotion, promotion.getPosition()), "Missing promotion preserves position");
    check(promotion.tryMove(*parseMove("b7a8n"), error), "Match promotion choice when accepting move");
    check(promotion.getPosition().board.at(0, 7).type == PieceType::Knight,
          "Apply selected promotion piece");
    check(promotion.undo() && samePosition(beforePromotion, promotion.getPosition()),
          "Undo promotion restores pawn, captured rook, and metadata");

    // No further moves are accepted after checkmate.
    Game mate;
    for (const char* text : {"f2f3", "e7e5", "g2g4", "d8h4"}) {
        check(mate.tryMove(*parseMove(text), error), "Play Fool's Mate");
    }
    check(mate.getStatus() == GameStatus::Checkmate, "Recognize checkmate after accepted move");
    const Position checkmate = mate.getPosition();
    check(!mate.tryMove(*parseMove("a2a3"), error) && !error.empty(),
          "Reject moves after checkmate");
    check(samePosition(checkmate, mate.getPosition()), "Post-game rejection preserves position");

    // Console errors and empty undo do not change the game.
    Game console;
    std::istringstream commands("bad input\ne2e5\nundo\ne2e4\nundo\nquit\ne7e5\n");
    std::ostringstream output;
    console.run(commands, output);
    check(samePosition(start, console.getPosition()), "Console undo restores the starting position");
    check(output.str().find("White to move") != std::string::npos &&
          output.str().find("Black to move") != std::string::npos,
          "Console displays whose turn it is");
    check(output.str().find("Invalid input") != std::string::npos &&
          output.str().find("cannot move") != std::string::npos &&
          output.str().find("No moves to undo") != std::string::npos,
          "Console explains malformed input, illegal moves, and empty undo");
    std::string unread;
    std::getline(commands, unread);
    check(unread == "e7e5", "Quit stops without consuming more input");

    // A final line without a newline is processed before end-of-file exits.
    Game eof;
    std::istringstream finalLine("e2e4");
    std::ostringstream eofOutput;
    eof.run(finalLine, eofOutput);
    check(eof.getPosition().sideToMove == Color::Black &&
          eof.getPosition().board.at(4, 3).type == PieceType::Pawn,
          "End-of-file exits cleanly after applying the final input line");

    Game checkGame;
    std::istringstream checkingMoves("e2e4\nd7d6\nf1b5\nquit\n");
    std::ostringstream checkOutput;
    checkGame.run(checkingMoves, checkOutput);
    check(checkGame.getStatus() == GameStatus::Check &&
          checkOutput.str().find("Check.") != std::string::npos,
          "Console reports check after a move");

    // Checkmate allows commands but rejects further moves; undo resumes play.
    std::istringstream afterMate("a2a3\nundo\nh7h6\nquit\n");
    std::ostringstream mateOutput;
    mate.run(afterMate, mateOutput);
    check(mateOutput.str().find("Checkmate! Black wins.") != std::string::npos &&
          mateOutput.str().find("Game over") != std::string::npos,
          "Console announces the winner and restricts input after checkmate");
    check(mate.getStatus() == GameStatus::Playing &&
          mate.getPosition().board.at(0, 1).type == PieceType::Pawn &&
          mate.getPosition().board.at(7, 5).type == PieceType::Pawn &&
          mate.getPosition().board.at(3, 7).type == PieceType::Queen,
          "Post-checkmate move is rejected, then undo permits a different legal reply");

    Game mateAtEof;
    std::istringstream matingMoves("f2f3\ne7e5\ng2g4\nd8h4\n");
    std::ostringstream resultOutput;
    mateAtEof.run(matingMoves, resultOutput);
    check(mateAtEof.getStatus() == GameStatus::Checkmate &&
          resultOutput.str().find("Checkmate! Black wins.") != std::string::npos,
          "Console reports checkmate even when input ends immediately afterward");

    // An accepted draw is a game result, not just an exit from the console.
    Game agreedDraw;
    std::istringstream acceptDraw("e2e4\ndraw\nY\nquit\nunused\n");
    std::ostringstream drawOutput;
    agreedDraw.run(acceptDraw, drawOutput);
    const Position drawnPosition = agreedDraw.getPosition();
    check(agreedDraw.getStatus() == GameStatus::DrawByAgreement && !agreedDraw.agreeDraw(),
          "Agreement is saved as a result and cannot be accepted twice");
    check(!agreedDraw.tryMove(*parseMove("e7e5"), error) && !error.empty() &&
          samePosition(drawnPosition, agreedDraw.getPosition()),
          "An agreed draw rejects later moves without changing the position");
    std::getline(acceptDraw, unread);
    check(unread == "unused", "Accepted draw still allows the quit command");
    check(agreedDraw.undo() && samePosition(start, agreedDraw.getPosition()) &&
          agreedDraw.tryMove(*parseMove("d2d4"), error),
          "Undo clears the agreement and restores the previous position");

    Game declinedDraw;
    std::istringstream declineDraw("draw\nn\ne2e4\nquit\n");
    std::ostringstream declineOutput;
    declinedDraw.run(declineDraw, declineOutput);
    check(declineOutput.str().find("Invalid input") == std::string::npos &&
          declinedDraw.getPosition().board.at(4, 3).type == PieceType::Pawn,
          "Declining a draw returns to move input without parsing the draw command");

    Game undoDraw;
    std::istringstream resumeDraw("draw\ny\ne2e4\nundo\nd2d4\nquit\n");
    std::ostringstream resumeOutput;
    undoDraw.run(resumeDraw, resumeOutput);
    check(undoDraw.getPosition().board.at(4, 1).type == PieceType::Pawn &&
          undoDraw.getPosition().board.at(3, 3).type == PieceType::Pawn,
          "After an initial-position agreement only undo or quit is allowed; undo resumes play");

    Game endedGame;
    std::istringstream drawAfterMate("f2f3\ne7e5\ng2g4\nd8h4\ndraw\ny\nquit\n");
    std::ostringstream endedOutput;
    endedGame.run(drawAfterMate, endedOutput);
    check(endedGame.getStatus() == GameStatus::Checkmate &&
          !endedGame.agreeDraw() &&
          endedOutput.str().find("Draw accepted") == std::string::npos &&
          endedOutput.str().find("Draw has been offered") == std::string::npos,
          "A draw cannot be offered after checkmate");

    Game unansweredDraw;
    std::istringstream noAnswer("draw\n");
    std::ostringstream noAnswerOutput;
    unansweredDraw.run(noAnswer, noAnswerOutput);
    check(unansweredDraw.getStatus() == GameStatus::Playing && !unansweredDraw.undo(),
          "End-of-file during an offer leaves the game and history unchanged");

    Game invalidAnswer;
    std::istringstream retryAnswer("draw\nmaybe\nN\ne2e4\nquit\n");
    std::ostringstream retryOutput;
    invalidAnswer.run(retryAnswer, retryOutput);
    check(retryOutput.str().find("Please enter y or n") != std::string::npos &&
          retryOutput.str().find("Invalid input") == std::string::npos &&
          invalidAnswer.getPosition().board.at(4, 3).type == PieceType::Pawn,
          "Invalid agreement answers are retried before play resumes");

    // Returning both knights twice repeats the starting position three times.
    const std::string repeatedMoves =
        "g1f3\ng8f6\nf3g1\nf6g8\ng1f3\ng8f6\nf3g1\nf6g8\n";
    Game repetition;
    std::istringstream claimRepetition(repeatedMoves + "maybe\nY\ne2e4\nquit\nunused\n");
    std::ostringstream repetitionOutput;
    repetition.run(claimRepetition, repetitionOutput);
    check(repetition.getStatus() == GameStatus::DrawByRepetition &&
          !repetition.claimThreefoldDraw() && !repetition.agreeDraw() &&
          !repetition.tryMove(*parseMove("e2e4"), error) &&
          repetition.getPosition().board.at(4, 1).type == PieceType::Pawn,
          "Claiming repetition ends play and preserves the board");
    check(repetitionOutput.str().find("Threefold repetition detected") != std::string::npos &&
          repetitionOutput.str().find("Please enter y or n") != std::string::npos,
          "Threefold claim prompt retries an invalid answer");
    std::getline(claimRepetition, unread);
    check(unread == "unused", "Claimed repetition still allows quit");
    check(repetition.undo() && repetition.tryMove(*parseMove("b8c6"), error),
          "Undo clears the repetition result and restores Black's previous turn");

    Game declinedRepetition;
    std::istringstream declineRepetition(repeatedMoves + "n\ne2e4\nquit\n");
    std::ostringstream declinedRepetitionOutput;
    declinedRepetition.run(declineRepetition, declinedRepetitionOutput);
    check(declinedRepetition.getPosition().board.at(4, 3).type == PieceType::Pawn &&
          declinedRepetition.getStatus() == GameStatus::Playing,
          "Declining a repetition claim allows play to continue");

    Game unansweredRepetition;
    std::istringstream repetitionEof(repeatedMoves);
    std::ostringstream repetitionEofOutput;
    unansweredRepetition.run(repetitionEof, repetitionEofOutput);
    check(unansweredRepetition.getStatus() == GameStatus::Playing,
          "End-of-file at the claim prompt does not claim a draw");

    check(!rules::threeFoldRepetition({}, start) &&
          !rules::threeFoldRepetition({start}, start) &&
          rules::threeFoldRepetition({start, start}, start),
          "Threefold detection requires two previous matches plus the current position");

    Position unusedTarget = start;
    unusedTarget.enPassantTarget = Square{4, 2};
    unusedTarget.sideToMove = Color::Black;
    Position withoutTarget = unusedTarget;
    withoutTarget.enPassantTarget.reset();
    check(rules::isSamePosition(unusedTarget, withoutTarget),
          "An en passant target without a legal capture does not affect repetition");

    Position legalTarget = createStartingPosition();
    for (const char* text : {"e2e4", "a7a6", "e4e5", "d7d5"}) {
        applyMove(legalTarget, *parseMove(text));
    }
    withoutTarget = legalTarget;
    withoutTarget.enPassantTarget.reset();
    check(!rules::isSamePosition(legalTarget, withoutTarget),
          "A legal en passant capture makes positions different");

    Position pinnedTarget;
    pinnedTarget.board.setPiece(4, 4, {PieceType::King, Color::White});
    pinnedTarget.board.setPiece(5, 4, {PieceType::Pawn, Color::White});
    pinnedTarget.board.setPiece(6, 4, {PieceType::Pawn, Color::Black});
    pinnedTarget.board.setPiece(7, 4, {PieceType::Rook, Color::Black});
    pinnedTarget.board.setPiece(0, 7, {PieceType::King, Color::Black});
    pinnedTarget.enPassantTarget = Square{6, 5};
    withoutTarget = pinnedTarget;
    withoutTarget.enPassantTarget.reset();
    check(rules::isSamePosition(pinnedTarget, withoutTarget),
          "En passant exposing the king does not affect repetition");

    Position different = start;
    different.sideToMove = Color::Black;
    check(!rules::isSamePosition(start, different), "Repetition requires the same side to move");
    different = start;
    different.whiteCanCastleKingside = false;
    check(!rules::isSamePosition(start, different), "Repetition requires the same castling rights");
    different = start;
    different.halfmoveClock = 8;
    different.fullmoveNumber = 5;
    check(rules::isSamePosition(start, different), "Move counters do not affect repetition");

    if (failures != 0) {
        return 1;
    }
    std::cout << "All game move checks passed.\n";
}
