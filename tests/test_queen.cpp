#include <gtest/gtest.h>
#include "piece/queen.h"
#include "test_base.h"

class QueenTest : public TestBase<Queen> {};

TEST_F(QueenTest, QueenInCenterEmptyBoard) {
    Board board("8/8/8/4Q3/8/8/8/8 w - - 0 1");
    generateMoves(board, 3, 4);  // e5

    EXPECT_EQ(moves.size(), 27);  // 14 rook + 13 bishop moves
}

TEST_F(QueenTest, QueenInCorner) {
    Board board("Q7/8/8/8/8/8/8/8 w - - 0 1");
    generateMoves(board, 0, 0);  // a8

    EXPECT_EQ(moves.size(), 21);  // 14 rook + 7 diagonal
    EXPECT_TRUE(hasMove(7, 7));   // h1 diagonal
    EXPECT_TRUE(hasMove(0, 7));   // h8 horizontal
    EXPECT_TRUE(hasMove(7, 0));   // a1 vertical
}

TEST_F(QueenTest, QueenBlockedByOwnPieces) {
    Board board("8/8/3PBP2/3PQP2/3PNP2/8/8/8 b - - 0 1");
    generateMoves(board, 3, 4);  // e5 surrounded by ally pieces

    EXPECT_EQ(moves.size(), 0);
}

TEST_F(QueenTest, QueenCapturesEnemy) {
    Board board("8/8/8/4Qp2/8/8/8/8 w - - 0 1");
    generateMoves(board, 3, 4);  // e5, pawn on f5

    EXPECT_TRUE(hasMove(3, 5));   // capture f5
    EXPECT_FALSE(hasMove(3, 6)); // can't go past f5
}
