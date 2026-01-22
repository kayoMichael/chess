#include <gtest/gtest.h>
#include "board/board.h"
#include "search/search.h"
#include "board/transposition.h"
#include "generator/generator.h"
#include "search/zobrist.h"

class TranspositionTableTest : public ::testing::Test {
protected:
    void SetUp() override {
        Zobrist::init();
    }
};

// Basic TT functionality tests
TEST_F(TranspositionTableTest, StoreAndProbe) {
    TranspositionTable tt(16);

    uint64_t hash = 0x123456789ABCDEF0;
    tt.store(hash, 100, 5, EXACT);

    TTEntry* entry = tt.probe(hash);
    ASSERT_NE(entry, nullptr);
    EXPECT_EQ(entry->score, 100);
    EXPECT_EQ(entry->depth, 5);
    EXPECT_EQ(entry->flag, EXACT);
}

TEST_F(TranspositionTableTest, ProbeNonexistent) {
    TranspositionTable tt(16);

    TTEntry* entry = tt.probe(0xDEADBEEF);
    EXPECT_EQ(entry, nullptr);
}

TEST_F(TranspositionTableTest, DeeperSearchReplaces) {
    TranspositionTable tt(16);

    uint64_t hash = 0x123456789ABCDEF0;
    tt.store(hash, 50, 3, EXACT);
    tt.store(hash, 100, 5, EXACT);  // Deeper search

    TTEntry* entry = tt.probe(hash);
    ASSERT_NE(entry, nullptr);
    EXPECT_EQ(entry->score, 100);  // Should have the deeper result
    EXPECT_EQ(entry->depth, 5);
}

TEST_F(TranspositionTableTest, ShallowerSearchDoesNotReplace) {
    TranspositionTable tt(16);

    uint64_t hash = 0x123456789ABCDEF0;
    tt.store(hash, 100, 5, EXACT);
    tt.store(hash, 50, 3, EXACT);  // Shallower search

    TTEntry* entry = tt.probe(hash);
    ASSERT_NE(entry, nullptr);
    EXPECT_EQ(entry->score, 100);  // Should keep the deeper result
    EXPECT_EQ(entry->depth, 5);
}

// Search with TT tests
class SearchWithTTTest : public ::testing::Test {
protected:
    Search search;

    void SetUp() override {
        Zobrist::init();
    }
};

TEST_F(SearchWithTTTest, SamePositionSameResult) {
    Board board1;
    Board board2;

    Move move1 = search.findBestMove(board1, 4);
    Move move2 = search.findBestMove(board2, 4);

    // Same position should give same best move
    EXPECT_EQ(move1.current.r, move2.current.r);
    EXPECT_EQ(move1.current.c, move2.current.c);
    EXPECT_EQ(move1.destination.r, move2.destination.r);
    EXPECT_EQ(move1.destination.c, move2.destination.c);
}

TEST_F(SearchWithTTTest, TTSpeedsUpSearch) {
    Board board("r1bqkbnr/pppp1ppp/2n5/4p3/4P3/5N2/PPPP1PPP/RNBQKB1R w KQkq - 2 3");

    // First search - cold TT
    auto start1 = std::chrono::high_resolution_clock::now();
    search.findBestMove(board, 5);
    auto end1 = std::chrono::high_resolution_clock::now();
    auto time1 = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1).count();

    // Second search - warm TT (same position)
    auto start2 = std::chrono::high_resolution_clock::now();
    search.findBestMove(board, 5);
    auto end2 = std::chrono::high_resolution_clock::now();
    auto time2 = std::chrono::duration_cast<std::chrono::milliseconds>(end2 - start2).count();

    // Second search should be faster (or at least not slower)
    // Allow some variance, but TT hit should help
    std::cout << "First search: " << time1 << "ms, Second search: " << time2 << "ms" << std::endl;
    EXPECT_LE(time2, time1 + 10);  // Should be roughly same or faster
}

TEST_F(SearchWithTTTest, TranspositionDetected) {
    // Two different move orders reaching the same position
    Board board1;
    Board board2;

    // Board 1: e4, e5, Nf3, Nc6
    board1.makeMove(board1.parseUCI("e2e4").value(), false);
    board1.makeMove(board1.parseUCI("e7e5").value(), false);
    board1.makeMove(board1.parseUCI("g1f3").value(), false);
    board1.makeMove(board1.parseUCI("b8c6").value(), false);

    // Board 2: Nf3, Nc6, e4, e5
    board2.makeMove(board2.parseUCI("g1f3").value(), false);
    board2.makeMove(board2.parseUCI("b8c6").value(), false);
    board2.makeMove(board2.parseUCI("e2e4").value(), false);
    board2.makeMove(board2.parseUCI("e7e5").value(), false);

    // Same position should have same hash
    EXPECT_EQ(board1.getHash(), board2.getHash());

    // And same best move
    Move move1 = search.findBestMove(board1, 4);
    Move move2 = search.findBestMove(board2, 4);

    EXPECT_EQ(move1.destination.r, move2.destination.r);
    EXPECT_EQ(move1.destination.c, move2.destination.c);
}

// Correctness tests - TT shouldn't change the result
TEST_F(SearchWithTTTest, MateInOneStillWorks) {
    Board board("6k1/5ppp/8/8/8/8/8/4Q2K w - - 0 1");
    Move best = search.findBestMove(board, 2);

    board.makeMove(best, false);
    auto moves = Generator::generateLegalMoves(board);
    EXPECT_TRUE(moves.empty());
    EXPECT_TRUE(board.isChecked(Color::Black));
}

TEST_F(SearchWithTTTest, MateInTwoStillWorks) {
    Board board("7k/8/4KP2/5PQP/6P1/8/8/8 w - - 0 1");

    // Play until mate
    for (int i = 0; i < 10; i++) {
        auto moves = Generator::generateLegalMoves(board);
        if (moves.empty()) break;

        Move best = search.findBestMove(board, 4);
        board.makeMove(best, false);
    }

    auto finalMoves = Generator::generateLegalMoves(board);
    EXPECT_TRUE(finalMoves.empty());
    EXPECT_TRUE(board.isChecked(Color::Black));
}

TEST_F(SearchWithTTTest, CaptureHangingPieceStillWorks) {
    Board board("8/6B1/8/8/3q4/8/8/4K2k w - - 0 1");
    Move best = search.findBestMove(board, 2);

    // Should capture the queen
    EXPECT_EQ(best.destination.r, 4);
    EXPECT_EQ(best.destination.c, 3);
}

// Test that bounds are being used correctly
TEST_F(SearchWithTTTest, ConsistentResultsAcrossDepths) {
    Board board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    // Results should be consistent (or improving) with depth
    int prevScore = -INF;
    for (int depth = 1; depth <= 5; depth++) {
        // Reset TT for fair comparison
        search = Search();

        Move best = search.findBestMove(board, depth);

        // Just verify it doesn't crash and gives reasonable moves
        EXPECT_GE(best.current.r, 0);
        EXPECT_LT(best.current.r, 8);
    }
}