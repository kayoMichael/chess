#include <gtest/gtest.h>
#include "board/board.h"
#include "search/search.h"
#include "generator/generator.h"

class SearchTest : public ::testing::Test {
protected:
    Search search;
};

TEST_F(SearchTest, WhiteMateInOne) {
    Board board("6k1/5ppp/8/8/8/8/8/4Q2K w - - 0 1");
    Move best = search.findBestMove(board, 2);

    board.makeMove(best, false);
    auto moves = Generator::generateLegalMoves(board);
    EXPECT_TRUE(moves.empty());
    EXPECT_TRUE(board.isChecked(Color::Black));
}

TEST_F(SearchTest, BlackMateInOne) {
    Board board("4q2k/8/8/8/8/8/5PPP/6K1 b - - 0 1");
    Move best = search.findBestMove(board, 2);

    board.makeMove(best, false);
    auto moves = Generator::generateLegalMoves(board);
    EXPECT_TRUE(moves.empty());
    EXPECT_TRUE(board.isChecked(Color::White));
}

TEST_F(SearchTest, WhiteCapturesHangingQueen) {
    Board board("8/6B1/8/8/3q4/8/8/4K2k w - - 0 1");
    Move best = search.findBestMove(board, 2);

    EXPECT_EQ(best.destination.r, 4);
    EXPECT_EQ(best.destination.c, 3);
}

TEST_F(SearchTest, BlackCapturesHangingQueen) {
    Board board("4k2K/8/8/3Q4/8/8/6b1/8 b - - 0 1");
    Move best = search.findBestMove(board, 2);

    EXPECT_EQ(best.destination.r, 3);
    EXPECT_EQ(best.destination.c, 3);
}

TEST_F(SearchTest, EvaluateStartingPosition) {
    Board board;
    int score = Search::evaluate(board);
    EXPECT_EQ(score, 0);
}

TEST_F(SearchTest, EvaluateWhiteUpQueen) {
    Board board("4k3/8/8/8/8/8/8/4K2Q w - - 0 1");
    int score = Search::evaluate(board);
    EXPECT_GT(score, 800);
}

TEST_F(SearchTest, EvaluateBlackUpQueen) {
    Board board("4k2q/8/8/8/8/8/8/4K3 w - - 0 1");
    int score = Search::evaluate(board);
    EXPECT_LT(score, -800);
}

TEST_F(SearchTest, WhiteMateInTwo) {
    Board board("7k/8/4KP2/5PQP/6P1/8/8/8 w - - 0 1");
    Move best = search.findBestMove(board, 4);

    board.makeMove(best, false);
    Move blackReply = search.findBestMove(board, 3);
    board.makeMove(blackReply, false);
    Move whiteMate = search.findBestMove(board, 2);
    board.makeMove(whiteMate, false);

    auto moves = Generator::generateLegalMoves(board);

    EXPECT_TRUE(moves.empty());
    EXPECT_TRUE(board.isChecked(Color::Black));
}

TEST_F(SearchTest, WhiteAvoidsStalemate) {
    Board board("k7/2Q5/1K6/8/8/8/8/8 w - - 0 1");
    Move best = search.findBestMove(board, 4);

    board.makeMove(best, false);
    auto moves = Generator::generateLegalMoves(board);
    bool isStalemate = moves.empty() && !board.isChecked(Color::Black);
    EXPECT_FALSE(isStalemate);
}

TEST_F(SearchTest, DeeperSearchFindsMateFaster) {
    Board board("6k1/5ppp/8/8/8/8/8/4Q2K w - - 0 1");

    Move depth2 = search.findBestMove(board, 2);
    Move depth4 = search.findBestMove(board, 4);

    EXPECT_EQ(depth2.destination.r, depth4.destination.r);
    EXPECT_EQ(depth2.destination.c, depth4.destination.c);
}