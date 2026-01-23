#include <gtest/gtest.h>
#include "board/board.h"
#include "search/search.h"
#include "generator/generator.h"
#include "search/zobrist.h"

class SearchTest : public ::testing::Test {
protected:
    Search search;
    void SetUp() override {
        Zobrist::init();
    }
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
    Move best = search.findBestMove(board, 4);

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

TEST_F(SearchTest, KnightOnRimIsDim) {
    // Knight on h4 vs knight on e4 - central knight should be better
    Board board_rim("4k3/8/8/8/7N/8/8/4K3 w - - 0 1");
    Board board_center("4k3/8/8/8/4N3/8/8/4K3 w - - 0 1");

    int score_rim = Search::evaluate(board_rim);
    int score_center = Search::evaluate(board_center);

    EXPECT_GT(score_center, score_rim);
}

TEST_F(SearchTest, KnightInCornerWorst) {
    Board board_corner("4k3/8/8/8/8/8/8/N3K3 w - - 0 1");
    Board board_edge("4k3/8/8/8/8/8/8/1N2K3 w - - 0 1");

    int score_corner = Search::evaluate(board_corner);
    int score_edge = Search::evaluate(board_edge);

    EXPECT_GT(score_edge, score_corner);
}

TEST_F(SearchTest, BishopMobilityMatters) {
    // Open bishop vs trapped bishop
    Board board_open("4k3/8/8/8/8/8/2P1K3/2B5 w - - 0 1");
    Board board_blocked("4k3/8/8/8/8/8/2P1K3/3B4 w - - 0 1");

    int score_open = Search::evaluate(board_open);
    int score_blocked = Search::evaluate(board_blocked);

    EXPECT_GT(score_open, score_blocked);
}

TEST_F(SearchTest, RookMobilityMatters) {
    // Open rook vs rook blocked by own pieces
    Board board_open("4k3/8/8/4P3/3R4/8/8/4K3 w - - 0 1");
    Board board_blocked("4k3/8/8/3P4/3R4/8/8/4K3 w - - 0 1");

    int score_open = Search::evaluate(board_open);
    int score_blocked = Search::evaluate(board_blocked);

    EXPECT_GT(score_open, score_blocked);
}

TEST_F(SearchTest, PassedPawnBonus) {
    // White pawn with no enemy pawns ahead or on adjacent files
    Board board_passed("4k3/8/8/8/3P4/8/8/4K3 w - - 0 1");
    // White pawn blocked by enemy pawn on same file
    Board board_blocked("4k3/3p4/8/8/3P4/8/8/4K3 w - - 0 1");

    int score_passed = Search::evaluate(board_passed);
    int score_blocked = Search::evaluate(board_blocked);

    EXPECT_GT(score_passed, score_blocked);
}

TEST_F(SearchTest, AdvancedPassedPawnWorthMore) {
    // Passed pawn on rank 6 vs rank 3
    Board board_advanced("4k3/8/3P4/8/8/8/8/4K3 w - - 0 1");
    Board board_back("4k3/8/8/8/8/3P4/8/4K3 w - - 0 1");

    int score_advanced = Search::evaluate(board_advanced);
    int score_back = Search::evaluate(board_back);

    EXPECT_GT(score_advanced, score_back);
}

TEST_F(SearchTest, PawnBlockedByAdjacentFilePawn) {
    // Pawn with enemy pawn on adjacent file ahead - not a passer
    Board board_not_passed("4k3/2p5/8/8/3P4/8/8/4K3 w - - 0 1");
    Board board_passed("4k3/8/8/8/3P4/8/8/4K3 w - - 0 1");

    int score_not_passed = Search::evaluate(board_not_passed);
    int score_passed = Search::evaluate(board_passed);

    EXPECT_GT(score_passed, score_not_passed);
}

TEST_F(SearchTest, KingInCenterBadEarlyGame) {
    // Full material - king in center should be punished
    Board board_center("r1bqkbnr/pppppppp/2n5/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    Board board_castled("r1bqkbnr/pppppppp/2n5/8/8/8/PPPPPPPP/RNBQBRKN b kq - 0 1");

    int score_center = Search::evaluate(board_center);
    int score_castled = Search::evaluate(board_castled);

    EXPECT_GT(score_castled, score_center);
}

TEST_F(SearchTest, KingCentralizationGoodEndgame) {
    // King and pawn endgame - central king better
    Board board_center("4k3/8/8/8/3K4/8/8/8 w - - 0 1");
    Board board_corner("4k3/8/8/8/8/8/8/K7 w - - 0 1");

    int score_center = Search::evaluate(board_center);
    int score_corner = Search::evaluate(board_corner);

    EXPECT_GT(score_center, score_corner);
}

TEST_F(SearchTest, StartingPositionIsEarlyPhase) {
    Board board;
    // Starting position should be evaluated with early game weights
    // This is implicit - just checking eval doesn't crash
    int score = Search::evaluate(board);
    EXPECT_EQ(score, 0);  // Symmetric position
}

TEST_F(SearchTest, EndgamePhaseDetection) {
    // Just kings - should be endgame phase
    Board board("4k3/8/8/8/8/8/8/4K3 w - - 0 1");
    int score = Search::evaluate(board);
    // Score should be 0 (equal material), just verifying no crash
    EXPECT_EQ(score, 0);
}

TEST_F(SearchTest, EarlyQueenDevelopmentPunished) {
    // Queen on h5 early (scholars mate style) vs queen on d1
    Board board_aggressive("rnbqkbnr/pppppppp/8/7Q/8/8/PPPPPPPP/RNB1KBNR w KQkq - 0 1");
    Board board_home("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    int score_aggressive = Search::evaluate(board_aggressive);
    int score_home = Search::evaluate(board_home);

    EXPECT_GT(score_home, score_aggressive);
}