#include <gtest/gtest.h>
#include "piece/knight.h"
#include "test_base.h"

class KnightTest : public TestBase<Knight> {};

TEST_F(KnightTest, KnightInCenterHasEightMoves) {
    Board board("8/8/8/4N3/8/8/8/8 w - - 0 1");
    generateMoves(board, 3, 4);  // e5

    EXPECT_EQ(moves.size(), 8);
}

TEST_F(KnightTest, KnightInCornerHasTwoMoves) {
    Board board("N7/8/8/8/8/8/8/8 w - - 0 1");
    generateMoves(board, 0, 0);  // a8

    EXPECT_EQ(moves.size(), 2);
    EXPECT_TRUE(hasMove(1, 2));  // c7
    EXPECT_TRUE(hasMove(2, 1));  // b6
}

TEST_F(KnightTest, KnightOnEdgeHasFourMoves) {
    Board board("4N3/8/8/8/8/8/8/8 w - - 0 1");
    generateMoves(board, 0, 4);  // e8

    EXPECT_EQ(moves.size(), 4);
}

TEST_F(KnightTest, KnightJumpsOverPieces) {
    Board board("8/3PPP2/3PNP2/3PPP2/8/8/8/8 w - - 0 1");
    generateMoves(board, 2, 4);  // e6 surrounded by pawns

    EXPECT_EQ(moves.size(), 8);  // Knight jumps over all pawns
}