#include "../include/Console.h"

#include <array>
#include <iostream>

int main() {
    struct ValidCase {
        const char* text;
        int fromFile;
        int fromRank;
        int toFile;
        int toRank;
        PieceType promotion = PieceType::None;
    };

    const std::array validCases{
        ValidCase{"e2e4", 4, 1, 4, 3},
        ValidCase{"a1h8", 0, 0, 7, 7},
        ValidCase{"h8a1", 7, 7, 0, 0},
        ValidCase{"e2e5", 4, 1, 4, 4},
        ValidCase{"a1a1", 0, 0, 0, 0},
        ValidCase{"e7e8", 4, 6, 4, 7},
        ValidCase{"e7e8q", 4, 6, 4, 7, PieceType::Queen},
        ValidCase{"e7e8r", 4, 6, 4, 7, PieceType::Rook},
        ValidCase{"e7e8b", 4, 6, 4, 7, PieceType::Bishop},
        ValidCase{"e7e8n", 4, 6, 4, 7, PieceType::Knight},
        ValidCase{"d2e1n", 3, 1, 4, 0, PieceType::Knight}
    };

    int failures = 0;
    for (const auto& test : validCases) {
        const auto move = parseMove(test.text);
        if (!move || move->from.file != test.fromFile ||
            move->from.rank != test.fromRank ||
            move->to.file != test.toFile || move->to.rank != test.toRank ||
            move->promotion != test.promotion) {
            std::cerr << "Incorrect parsing: " << test.text << '\n';
            ++failures;
        }
    }

    const std::array invalidCases{
        "", "e2", "e2e4extra", "e7e8k", "e7e8p", "e7e8Q", "e7e8qq", "i2e4", "e2i4",
        "`2e4", "e2`4", "e0e4", "e2e0", "e9e4", "e2e9",
        "E2e4", "e2E4", " e2e4", "e2e4 ", "e2 e4", "e2\ne4",
        "e2\te4", "quit", "exit"
    };

    for (const auto* text : invalidCases) {
        if (parseMove(text)) {
            std::cerr << "Accepted malformed input: " << text << '\n';
            ++failures;
        }
    }

    if (failures != 0) {
        return 1;
    }

    std::cout << "All move parsing checks passed.\n";
}
