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
    };

    const std::array validCases{
        ValidCase{"e2e4", 4, 1, 4, 3},
        ValidCase{"a1h8", 0, 0, 7, 7},
        ValidCase{"h8a1", 7, 7, 0, 0},
        ValidCase{"e2e5", 4, 1, 4, 4},
        ValidCase{"a1a1", 0, 0, 0, 0}
    };

    int failures = 0;
    for (const auto& test : validCases) {
        const auto move = parseMove(test.text);
        if (!move || move->from.file != test.fromFile ||
            move->from.rank != test.fromRank ||
            move->to.file != test.toFile || move->to.rank != test.toRank ||
            move->promotion != PieceType::None) {
            std::cerr << "Incorrect parsing: " << test.text << '\n';
            ++failures;
        }
    }

    const std::array invalidCases{
        "", "e2", "e2e4extra", "e7e8q", "i2e4", "e2i4",
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
