#include <gtest/gtest.h>
#include "board/board.h"
#include "piece/king.h"

class KingTest : public ::testing::Test {
protected:
    King king;
    std::vector<Move> moves;

    void generateMoves(Board& board, int row, int col) {
        moves.clear();
        king.generateMoves(board, row, col, moves);
    }

    bool hasMove(int fromR, int fromC, int toR, int toC) {
        for (const auto& m : moves) {
            if (m.current.r == fromR && m.current.c == fromC &&
                m.destination.r == toR && m.destination.c == toC) {
                return true;
            }
        }
        return false;
    }

    bool hasCastle(int fromR, int fromC, int toR, int toC) {
        for (const auto& m : moves) {
            if (m.current.r == fromR && m.current.c == fromC &&
                m.destination.r == toR && m.destination.c == toC &&
                m.type == MoveType::Castle) {
                return true;
            }
        }
        return false;
    }

    int countMovesOfType(MoveType type) {
        int count = 0;
        for (const auto& m : moves) {
            if (m.type == type) count++;
        }
        return count;
    }
};

TEST_F(KingTest, KingInCenterHasEightMoves) {
    Board board("8/8/8/4K3/8/8/8/8 w - - 0 1");
    generateMoves(board, 3, 4);  // e5

    EXPECT_EQ(moves.size(), 8);
    EXPECT_TRUE(hasMove(3, 4, 2, 3));  // d6
    EXPECT_TRUE(hasMove(3, 4, 2, 4));  // e6
    EXPECT_TRUE(hasMove(3, 4, 2, 5));  // f6
    EXPECT_TRUE(hasMove(3, 4, 3, 3));  // d5
    EXPECT_TRUE(hasMove(3, 4, 3, 5));  // f5
    EXPECT_TRUE(hasMove(3, 4, 4, 3));  // d4
    EXPECT_TRUE(hasMove(3, 4, 4, 4));  // e4
    EXPECT_TRUE(hasMove(3, 4, 4, 5));  // f4
}

TEST_F(KingTest, KingInCornerHasThreeMoves) {
    Board board("K7/8/8/8/8/8/8/8 w - - 0 1");
    generateMoves(board, 0, 0);  // a8

    EXPECT_EQ(moves.size(), 3);
    EXPECT_TRUE(hasMove(0, 0, 0, 1));  // b8
    EXPECT_TRUE(hasMove(0, 0, 1, 0));  // a7
    EXPECT_TRUE(hasMove(0, 0, 1, 1));  // b7
}

TEST_F(KingTest, KingOnEdgeHasFiveMoves) {
    Board board("4K3/8/8/8/8/8/8/8 w - - 0 1");
    generateMoves(board, 0, 4);  // e8

    EXPECT_EQ(moves.size(), 5);
    EXPECT_TRUE(hasMove(0, 4, 0, 3));  // d8
    EXPECT_TRUE(hasMove(0, 4, 0, 5));  // f8
    EXPECT_TRUE(hasMove(0, 4, 1, 3));  // d7
    EXPECT_TRUE(hasMove(0, 4, 1, 4));  // e7
    EXPECT_TRUE(hasMove(0, 4, 1, 5));  // f7
}

TEST_F(KingTest, KingCanCaptureEnemy) {
    Board board("8/8/8/3pK3/8/8/8/8 w - - 0 1");
    generateMoves(board, 3, 4);  // e5, d5 has black pawn

    EXPECT_TRUE(hasMove(3, 4, 3, 3));  // Kxd5
}

TEST_F(KingTest, KingCannotCaptureOwnPiece) {
    Board board("8/8/8/3PK3/8/8/8/8 w - - 0 1");
    generateMoves(board, 3, 4);  // e5, d5 has white pawn

    EXPECT_FALSE(hasMove(3, 4, 3, 3));  // Cannot take own pawn
    EXPECT_EQ(moves.size(), 7);
}

TEST_F(KingTest, KingSurroundedByOwnPieces) {
    Board board("8/8/3PPP2/3PKP2/3PPP2/8/8/8 w - - 0 1");
    generateMoves(board, 3, 4);  // e5 surrounded

    EXPECT_EQ(moves.size(), 0);
}

TEST_F(KingTest, WhiteKingsideCastleAvailable) {
    Board board("8/8/8/8/8/8/8/4K2R w K - 0 1");
    generateMoves(board, 7, 4);  // e1

    EXPECT_TRUE(hasCastle(7, 4, 7, 6));  // O-O
}

TEST_F(KingTest, WhiteKingsideCastleBlockedByPiece) {
    Board board("8/8/8/8/8/8/8/4KN1R w K - 0 1");
    generateMoves(board, 7, 4);  // e1, f1 blocked

    EXPECT_FALSE(hasCastle(7, 4, 7, 6));
}

TEST_F(KingTest, WhiteKingsideCastleBlockedByPieceOnG1) {
    Board board("8/8/8/8/8/8/8/4K1NR w K - 0 1");
    generateMoves(board, 7, 4);  // e1, g1 blocked

    EXPECT_FALSE(hasCastle(7, 4, 7, 6));
}

TEST_F(KingTest, WhiteKingsideCastleKingMoved) {
    Board board("8/8/8/8/8/8/8/4K2R w - - 0 1");  // No castling rights
    generateMoves(board, 7, 4);

    EXPECT_FALSE(hasCastle(7, 4, 7, 6));
}

TEST_F(KingTest, WhiteKingsideCastleRookMoved) {
    Board board("8/8/8/8/8/8/8/4K2R w Q - 0 1");  // Only queenside rights
    generateMoves(board, 7, 4);

    EXPECT_FALSE(hasCastle(7, 4, 7, 6));
}

TEST_F(KingTest, WhiteKingsideCastleThroughCheck) {
    Board board("5r2/8/8/8/8/8/8/4K2R w K - 0 1");  // Rook attacks f1
    generateMoves(board, 7, 4);

    EXPECT_FALSE(hasCastle(7, 4, 7, 6));
}

TEST_F(KingTest, WhiteKingsideCastleIntoCheck) {
    Board board("6r1/8/8/8/8/8/8/4K2R w K - 0 1");  // Rook attacks g1
    generateMoves(board, 7, 4);

    EXPECT_FALSE(hasCastle(7, 4, 7, 6));
}

TEST_F(KingTest, WhiteKingsideCastleWhileInCheck) {
    Board board("4r3/8/8/8/8/8/8/4K2R w K - 0 1");  // Rook attacks e1
    generateMoves(board, 7, 4);

    EXPECT_FALSE(hasCastle(7, 4, 7, 6));
}

TEST_F(KingTest, WhiteQueensideCastleAvailable) {
    Board board("8/8/8/8/8/8/8/R3K3 w Q - 0 1");
    generateMoves(board, 7, 4);  // e1

    EXPECT_TRUE(hasCastle(7, 4, 7, 2));  // O-O-O
}

TEST_F(KingTest, WhiteQueensideCastleBlockedByPieceOnD1) {
    Board board("8/8/8/8/8/8/8/R2NK3 w Q - 0 1");
    generateMoves(board, 7, 4);

    EXPECT_FALSE(hasCastle(7, 4, 7, 2));
}

TEST_F(KingTest, WhiteQueensideCastleBlockedByPieceOnC1) {
    Board board("8/8/8/8/8/8/8/R1N1K3 w Q - 0 1");
    generateMoves(board, 7, 4);

    EXPECT_FALSE(hasCastle(7, 4, 7, 2));
}

TEST_F(KingTest, WhiteQueensideCastleBlockedByPieceOnB1) {
    Board board("8/8/8/8/8/8/8/RN2K3 w Q - 0 1");
    generateMoves(board, 7, 4);

    EXPECT_FALSE(hasCastle(7, 4, 7, 2));
}

TEST_F(KingTest, WhiteQueensideCastleThroughCheck) {
    Board board("3r4/8/8/8/8/8/8/R3K3 w Q - 0 1");  // Rook attacks d1
    generateMoves(board, 7, 4);

    EXPECT_FALSE(hasCastle(7, 4, 7, 2));
}

TEST_F(KingTest, WhiteQueensideCastleIntoCheck) {
    Board board("2r5/8/8/8/8/8/8/R3K3 w Q - 0 1");  // Rook attacks c1
    generateMoves(board, 7, 4);

    EXPECT_FALSE(hasCastle(7, 4, 7, 2));
}

TEST_F(KingTest, WhiteQueensideCastleB1Attacked) {
    // b1 attacked is OK - king doesn't pass through it
    Board board("1r6/8/8/8/8/8/8/R3K3 w Q - 0 1");  // Rook attacks b1
    generateMoves(board, 7, 4);

    EXPECT_TRUE(hasCastle(7, 4, 7, 2));  // Should still be legal
}

TEST_F(KingTest, BlackKingsideCastleAvailable) {
    Board board("4k2r/8/8/8/8/8/8/8 b k - 0 1");
    generateMoves(board, 0, 4);  // e8

    EXPECT_TRUE(hasCastle(0, 4, 0, 6));  // O-O
}

TEST_F(KingTest, BlackKingsideCastleBlocked) {
    Board board("4kn1r/8/8/8/8/8/8/8 b k - 0 1");
    generateMoves(board, 0, 4);

    EXPECT_FALSE(hasCastle(0, 4, 0, 6));
}

TEST_F(KingTest, BlackKingsideCastleThroughCheck) {
    Board board("4k2r/8/8/8/8/8/8/5R2 b k - 0 1");  // Rook attacks f8
    generateMoves(board, 0, 4);

    EXPECT_FALSE(hasCastle(0, 4, 0, 6));
}

TEST_F(KingTest, BlackQueensideCastleAvailable) {
    Board board("r3k3/8/8/8/8/8/8/8 b q - 0 1");
    generateMoves(board, 0, 4);  // e8

    EXPECT_TRUE(hasCastle(0, 4, 0, 2));  // O-O-O
}

TEST_F(KingTest, BlackQueensideCastleBlocked) {
    Board board("r2nk3/8/8/8/8/8/8/8 b q - 0 1");
    generateMoves(board, 0, 4);

    EXPECT_FALSE(hasCastle(0, 4, 0, 2));
}

TEST_F(KingTest, WhiteBothCastlesAvailable) {
    Board board("8/8/8/8/8/8/8/R3K2R w KQ - 0 1");
    generateMoves(board, 7, 4);

    EXPECT_TRUE(hasCastle(7, 4, 7, 6));  // O-O
    EXPECT_TRUE(hasCastle(7, 4, 7, 2));  // O-O-O
    EXPECT_EQ(countMovesOfType(MoveType::Castle), 2);
}

TEST_F(KingTest, BlackBothCastlesAvailable) {
    Board board("r3k2r/8/8/8/8/8/8/8 b kq - 0 1");
    generateMoves(board, 0, 4);

    EXPECT_TRUE(hasCastle(0, 4, 0, 6));  // O-O
    EXPECT_TRUE(hasCastle(0, 4, 0, 2));  // O-O-O
    EXPECT_EQ(countMovesOfType(MoveType::Castle), 2);
}

TEST_F(KingTest, NoCastlingFromNonStandardPosition) {
    // King not on e-file
    Board board("8/8/8/8/8/8/8/3K3R w K - 0 1");
    generateMoves(board, 7, 3);  // d1

    EXPECT_EQ(countMovesOfType(MoveType::Castle), 0);
}