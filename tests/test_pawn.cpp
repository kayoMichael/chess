#include <gtest/gtest.h>
#include "board/board.h"
#include "piece/pawn.h"

class PawnTest : public ::testing::Test {
protected:
    Pawn pawn;
    std::vector<Move> moves;

    void generateMoves(Board& board, int row, int col) {
        moves.clear();
        pawn.generateMoves(board, row, col, moves);
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

    bool hasPromotion(int fromR, int fromC, int toR, int toC, PieceKind promo) {
        for (const auto& m : moves) {
            if (m.current.r == fromR && m.current.c == fromC &&
                m.destination.r == toR && m.destination.c == toC &&
                m.type == MoveType::Promotion && m.promotion == promo) {
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

TEST_F(PawnTest, WhitePawnStartingPosition) {
    Board board;  // Default starting position
    generateMoves(board, 6, 4);  // e2 pawn

    EXPECT_EQ(moves.size(), 2);
    EXPECT_TRUE(hasMove(6, 4, 5, 4));  // e3
    EXPECT_TRUE(hasMove(6, 4, 4, 4));  // e4
}

TEST_F(PawnTest, BlackPawnStartingPosition) {
    Board board("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq - 0 1");
    generateMoves(board, 1, 4);  // e7 pawn

    EXPECT_EQ(moves.size(), 2);
    EXPECT_TRUE(hasMove(1, 4, 2, 4));  // e6
    EXPECT_TRUE(hasMove(1, 4, 3, 4));  // e5
}

TEST_F(PawnTest, WhitePawnSinglePushFromMiddle) {
    Board board("8/8/8/4P3/8/8/8/8 w - - 0 1");
    generateMoves(board, 3, 4);  // e5 pawn

    EXPECT_EQ(moves.size(), 1);
    EXPECT_TRUE(hasMove(3, 4, 2, 4));  // e6
}

TEST_F(PawnTest, PawnBlockedCannotMove) {
    Board board("8/8/4p3/4P3/8/8/8/8 w - - 0 1");
    generateMoves(board, 3, 4);  // e5 blocked by e6 pawn

    EXPECT_EQ(moves.size(), 0);
}

TEST_F(PawnTest, DoublePushBlockedByPieceOneSquareAhead) {
    Board board("8/8/8/8/8/4p3/4P3/8 w - - 0 1");
    generateMoves(board, 6, 4);  // e2 pawn, e3 blocked

    EXPECT_EQ(moves.size(), 0);
}

TEST_F(PawnTest, DoublePushBlockedByPieceTwoSquaresAhead) {
    Board board("8/8/8/8/4p3/8/4P3/8 w - - 0 1");
    generateMoves(board, 6, 4);  // e2 pawn, e4 blocked

    EXPECT_EQ(moves.size(), 1);
    EXPECT_TRUE(hasMove(6, 4, 5, 4));  // Only e3
}

TEST_F(PawnTest, WhitePawnCaptureLeft) {
    Board board("8/8/8/3p4/4P3/8/8/8 w - - 0 1");
    generateMoves(board, 4, 4);  // e4 pawn

    EXPECT_TRUE(hasMove(4, 4, 3, 3));  // exd5
    EXPECT_TRUE(hasMove(4, 4, 3, 4));  // e5
    EXPECT_EQ(moves.size(), 2);
}

TEST_F(PawnTest, WhitePawnCaptureRight) {
    Board board("8/8/8/5p2/4P3/8/8/8 w - - 0 1");
    generateMoves(board, 4, 4);  // e4 pawn

    EXPECT_TRUE(hasMove(4, 4, 3, 5));  // exf5
    EXPECT_TRUE(hasMove(4, 4, 3, 4));  // e5
    EXPECT_EQ(moves.size(), 2);
}

TEST_F(PawnTest, WhitePawnCaptureBoth) {
    Board board("8/8/8/3ppp2/4P3/8/8/8 w - - 0 1");
    generateMoves(board, 4, 4);  // e4 pawn

    EXPECT_TRUE(hasMove(4, 4, 3, 3));  // exd5
    EXPECT_TRUE(hasMove(4, 4, 3, 5));  // exf5
    EXPECT_FALSE(hasMove(4, 4, 3, 4)); // e5 blocked
    EXPECT_EQ(moves.size(), 2);
}

TEST_F(PawnTest, CannotCaptureOwnPiece) {
    Board board("8/8/8/3P4/4P3/8/8/8 w - - 0 1");
    generateMoves(board, 4, 4);  // e4 pawn, d5 is friendly

    EXPECT_FALSE(hasMove(4, 4, 3, 3));
    EXPECT_TRUE(hasMove(4, 4, 3, 4));  // e5
    EXPECT_EQ(moves.size(), 1);
}

TEST_F(PawnTest, CannotCaptureDiagonallyIntoEmpty) {
    Board board("8/8/8/8/4P3/8/8/8 w - - 0 1");
    generateMoves(board, 4, 4);  // e4 pawn

    EXPECT_FALSE(hasMove(4, 4, 3, 3));  // No capture to d5
    EXPECT_FALSE(hasMove(4, 4, 3, 5));  // No capture to f5
    EXPECT_TRUE(hasMove(4, 4, 3, 4));   // e5
    EXPECT_EQ(moves.size(), 1);
}

TEST_F(PawnTest, EnPassantCaptureRight) {
    Board board("8/8/8/4Pp2/8/8/8/8 w - f6 0 1");
    generateMoves(board, 3, 4);  // e5 pawn, en passant on f6

    EXPECT_TRUE(hasMove(3, 4, 2, 5));  // exf6 en passant
    EXPECT_EQ(countMovesOfType(MoveType::EnPassant), 1);
}

TEST_F(PawnTest, EnPassantCaptureLeft) {
    Board board("8/8/8/3pP3/8/8/8/8 w - d6 0 1");
    generateMoves(board, 3, 4);  // e5 pawn, en passant on d6

    EXPECT_TRUE(hasMove(3, 4, 2, 3));  // exd6 en passant
    EXPECT_EQ(countMovesOfType(MoveType::EnPassant), 1);
}

TEST_F(PawnTest, BlackEnPassant) {
    Board board("8/8/8/8/3Pp3/8/8/8 b - d3 0 1");
    generateMoves(board, 4, 4);  // e4 black pawn

    EXPECT_TRUE(hasMove(4, 4, 5, 3));  // exd3 en passant
    EXPECT_EQ(countMovesOfType(MoveType::EnPassant), 1);
}

TEST_F(PawnTest, NoEnPassantWhenNotAdjacent) {
    Board board("8/8/8/4P3/8/8/8/8 w - a6 0 1");
    generateMoves(board, 3, 4);  // e5 pawn, en passant on a6 (not adjacent)

    EXPECT_EQ(countMovesOfType(MoveType::EnPassant), 0);
}

TEST_F(PawnTest, WhitePromotionStraight) {
    Board board("8/4P3/8/8/8/8/8/8 w - - 0 1");
    generateMoves(board, 1, 4);  // e7 pawn

    EXPECT_EQ(moves.size(), 4);
    EXPECT_TRUE(hasPromotion(1, 4, 0, 4, PieceKind::Queen));
    EXPECT_TRUE(hasPromotion(1, 4, 0, 4, PieceKind::Rook));
    EXPECT_TRUE(hasPromotion(1, 4, 0, 4, PieceKind::Bishop));
    EXPECT_TRUE(hasPromotion(1, 4, 0, 4, PieceKind::Knight));
}

TEST_F(PawnTest, WhitePromotionWithCapture) {
    Board board("3r4/4P3/8/8/8/8/8/8 w - - 0 1");
    generateMoves(board, 1, 4);  // e7 pawn, can capture d8

    EXPECT_EQ(moves.size(), 8);  // 4 promotions straight + 4 capture promotions
    EXPECT_TRUE(hasPromotion(1, 4, 0, 4, PieceKind::Queen));
    EXPECT_TRUE(hasPromotion(1, 4, 0, 3, PieceKind::Queen));  // exd8=Q
}

TEST_F(PawnTest, BlackPromotion) {
    Board board("8/8/8/8/8/8/4p3/8 b - - 0 1");
    generateMoves(board, 6, 4);  // e2 black pawn

    EXPECT_EQ(moves.size(), 4);
    EXPECT_TRUE(hasPromotion(6, 4, 7, 4, PieceKind::Queen));
    EXPECT_TRUE(hasPromotion(6, 4, 7, 4, PieceKind::Knight));
}

TEST_F(PawnTest, PromotionBlockedNoMoves) {
    Board board("4r3/4P3/8/8/8/8/8/8 w - - 0 1");
    generateMoves(board, 1, 4);  // e7 pawn blocked by e8

    EXPECT_EQ(countMovesOfType(MoveType::Promotion), 0);
}

TEST_F(PawnTest, PawnOnAFile) {
    Board board("8/8/8/8/P7/8/8/8 w - - 0 1");
    generateMoves(board, 4, 0);  // a4 pawn

    EXPECT_EQ(moves.size(), 1);
    EXPECT_TRUE(hasMove(4, 0, 3, 0));  // a5
}

TEST_F(PawnTest, PawnOnHFile) {
    Board board("8/8/8/8/7P/8/8/8 w - - 0 1");
    generateMoves(board, 4, 7);  // h4 pawn

    EXPECT_EQ(moves.size(), 1);
    EXPECT_TRUE(hasMove(4, 7, 3, 7));  // h5
}

TEST_F(PawnTest, PawnOnAFileCaptureRight) {
    Board board("8/8/8/1p6/P7/8/8/8 w - - 0 1");
    generateMoves(board, 4, 0);  // a4 pawn

    EXPECT_EQ(moves.size(), 2);
    EXPECT_TRUE(hasMove(4, 0, 3, 0));  // a5
    EXPECT_TRUE(hasMove(4, 0, 3, 1));  // axb5
}