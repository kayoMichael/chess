#include <gtest/gtest.h>
#include "piece/rook.h"

class RookTest : public ::testing::Test {
protected:
    Rook rook;
    std::vector<Move> moves;

    void generateMoves(Board& board, int row, int col) {
        moves.clear();
        rook.generateMoves(board, row, col, moves);
    }

    bool hasMove(int toR, int toC) {
        for (const auto& m : moves) {
            if (m.destination.r == toR && m.destination.c == toC) return true;
        }
        return false;
    }
};

TEST_F(RookTest, RookInCenterEmptyBoard) {
    Board board("8/8/8/4R3/8/8/8/8 w - - 0 1");
    generateMoves(board, 3, 4);  // e5

    EXPECT_EQ(moves.size(), 14);  // 7 vertical + 7 horizontal
}

TEST_F(RookTest, RookInCorner) {
    Board board("R7/8/8/8/8/8/8/8 w - - 0 1");
    generateMoves(board, 0, 0);  // a8

    EXPECT_EQ(moves.size(), 14);
    EXPECT_TRUE(hasMove(0, 7));  // h8
    EXPECT_TRUE(hasMove(7, 0));  // a1
}

TEST_F(RookTest, RookBlockedByOwnPieces) {
    Board board("8/8/8/2P1R1P1/8/8/8/8 w - - 0 1");
    generateMoves(board, 3, 4);  // e5, blocked by pawns on c5 and g5

    EXPECT_FALSE(hasMove(3, 2));  // c5 blocked
    EXPECT_FALSE(hasMove(3, 6));  // g5 blocked
    EXPECT_TRUE(hasMove(3, 3));   // d5 ok
    EXPECT_TRUE(hasMove(3, 5));   // f5 ok
}

TEST_F(RookTest, RookCapturesEnemy) {
    Board board("8/8/8/2p1R1p1/8/8/8/8 w - - 0 1");
    generateMoves(board, 3, 4);  // e5, can capture c5 and g5

    EXPECT_TRUE(hasMove(3, 2));   // capture c5
    EXPECT_TRUE(hasMove(3, 6));   // capture g5
    EXPECT_FALSE(hasMove(3, 1));  // can't go past c5
    EXPECT_FALSE(hasMove(3, 7));  // can't go past g5
}