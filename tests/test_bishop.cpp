#include <gtest/gtest.h>
#include "piece/bishop.h"
#include "test_base.h"

class BishopTest : public TestBase<Bishop> {};

TEST_F(BishopTest, BishopInCenterEmptyBoard) {
    Board board("8/8/8/4B3/8/8/8/8 w - - 0 1");
    generateMoves(board, 3, 4);  // e5

    EXPECT_EQ(moves.size(), 13);
}

TEST_F(BishopTest, BishopInCorner) {
    Board board("B7/8/8/8/8/8/8/8 w - - 0 1");
    generateMoves(board, 0, 0);  // a8

    EXPECT_EQ(moves.size(), 7);
    EXPECT_TRUE(hasMove(7, 7));  // h1
}

TEST_F(BishopTest, BishopBlockedByOwnPieces) {
    Board board("8/8/3P1P2/4B3/3P1P2/8/8/8 w - - 0 1");
    generateMoves(board, 3, 4);  // e5, diagonals blocked

    EXPECT_EQ(moves.size(), 0);
}

TEST_F(BishopTest, BishopCapturesEnemy) {
    Board board("8/8/5p2/4B3/8/8/8/8 w - - 0 1");
    generateMoves(board, 3, 4);  // e5, pawn on f6

    EXPECT_TRUE(hasMove(2, 5));   // capture f6
    EXPECT_FALSE(hasMove(1, 6)); // can't go past f6
}

TEST_F(BishopTest, LightSquareBishop) {
    Board board("8/8/8/8/8/8/8/5B2 w - - 0 1");
    generateMoves(board, 7, 5);  // f1 (light square)

    // All moves should be to light squares
    for (const auto& m : moves) {
        int squareColor = (m.destination.r + m.destination.c) % 2;
        int startColor = (7 + 5) % 2;
        EXPECT_EQ(squareColor, startColor);
    }
}