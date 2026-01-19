#include <gtest/gtest.h>
#include "board/board.h"

TEST(BoardTest, DefaultConstructorStartingPosition) {
    Board board;
    EXPECT_EQ(board.at(0, 0).kind, PieceKind::Rook);
    EXPECT_EQ(board.at(0, 0).color, Color::Black);
    EXPECT_EQ(board.at(7, 4).kind, PieceKind::King);
    EXPECT_EQ(board.at(7, 4).color, Color::White);
}

TEST(BoardTest, FENStartingPosition) {
    Board board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    EXPECT_EQ(board.at(0, 0).kind, PieceKind::Rook);
    EXPECT_EQ(board.getColor(), Color::White);
}

TEST(BoardTest, FENRoundTrip) {
    std::string fen = "r3k2r/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w KQkq - 0 1";
    Board board(fen);
    EXPECT_EQ(board.toFEN(), fen);
}

TEST(BoardTest, InvalidFENThrows) {
    EXPECT_THROW(Board("invalid"), std::invalid_argument);
    EXPECT_THROW(Board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP w KQkq - 0 1"), std::invalid_argument);
}

TEST(BoardTest, ParseUCIBasic) {
    Board board;
    auto move = board.parseUCI("e2e4");
    ASSERT_TRUE(move.has_value());
    EXPECT_EQ(move->current.r, 6);
    EXPECT_EQ(move->current.c, 4);
    EXPECT_EQ(move->destination.r, 4);
    EXPECT_EQ(move->destination.c, 4);
}

TEST(BoardTest, ParseUCIInvalidThrows) {
    Board board;
    EXPECT_THROW(board.parseUCI("e2"), std::invalid_argument);
    EXPECT_THROW(board.parseUCI("i2e4"), std::invalid_argument);
    EXPECT_THROW(board.parseUCI("e9e4"), std::invalid_argument);
}