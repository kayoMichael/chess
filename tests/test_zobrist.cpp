#include <gtest/gtest.h>
#include "board/board.h"
#include "search/zobrist.h"

class ZobristTest : public ::testing::Test {
protected:
    void SetUp() override {
        Zobrist::init();
    }
};

TEST_F(ZobristTest, StartingPositionConsistentHash) {
    Board board1;
    Board board2;
    EXPECT_EQ(board1.getHash(), board2.getHash());
}

TEST_F(ZobristTest, DifferentPositionsDifferentHash) {
    Board board1;
    Board board2("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1");
    EXPECT_NE(board1.getHash(), board2.getHash());
}

TEST_F(ZobristTest, MoveAndUndoRestoresHash) {
    Board board;
    uint64_t originalHash = board.getHash();

    Move e4 = board.parseUCI("e2e4").value();
    MoveUndo undo = board.makeMove(e4, false);

    EXPECT_NE(board.getHash(), originalHash);  // Hash should change

    board.undoMove(undo);

    EXPECT_EQ(board.getHash(), originalHash);  // Hash should restore
}

TEST_F(ZobristTest, SamePositionDifferentPathSameHash) {
    // Reach same position via different move orders
    Board board1;

    // Board 1: e4, e5, Nf3
    Move e4 = board1.parseUCI("e2e4").value();
    board1.makeMove(e4, false);
    Move e5 = board1.parseUCI("e7e5").value();
    board1.makeMove(e5, false);
    Move nf3 = board1.parseUCI("g1f3").value();
    board1.makeMove(nf3, false);

    Board board3("rnbqkbnr/pppp1ppp/8/4p3/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq - 1 2");

    EXPECT_EQ(board1.getHash(), board3.getHash());
}

TEST_F(ZobristTest, CaptureAndUndoRestoresHash) {
    Board board("rnbqkbnr/ppp1pppp/8/3p4/4P3/8/PPPP1PPP/RNBQKBNR w KQkq d6 0 2");
    uint64_t originalHash = board.getHash();

    Move exd5 = board.parseUCI("e4d5").value();
    MoveUndo undo = board.makeMove(exd5, false);

    board.undoMove(undo);

    EXPECT_EQ(board.getHash(), originalHash);
}

TEST_F(ZobristTest, CastlingAndUndoRestoresHash) {
    Board board("r1bqk2r/pppp1ppp/2n2n2/2b1p3/2B1P3/5N2/PPPP1PPP/RNBQK2R w KQkq - 4 4");
    uint64_t originalHash = board.getHash();

    Move castle = board.parseUCI("e1g1").value();
    MoveUndo undo = board.makeMove(castle, false);

    EXPECT_NE(board.getHash(), originalHash);

    board.undoMove(undo);

    EXPECT_EQ(board.getHash(), originalHash);
}

TEST_F(ZobristTest, EnPassantAndUndoRestoresHash) {
    Board board("rnbqkbnr/ppp1p1pp/8/3pPp2/8/8/PPPP1PPP/RNBQKBNR w KQkq f6 0 3");
    uint64_t originalHash = board.getHash();

    Move ep = board.parseUCI("e5f6").value();
    MoveUndo undo = board.makeMove(ep, false);

    EXPECT_NE(board.getHash(), originalHash);

    board.undoMove(undo);

    EXPECT_EQ(board.getHash(), originalHash);
}

TEST_F(ZobristTest, PromotionAndUndoRestoresHash) {
    Board board("8/P7/8/8/8/8/8/4K2k w - - 0 1");
    uint64_t originalHash = board.getHash();

    Move promo = board.parseUCI("a7a8q").value();
    MoveUndo undo = board.makeMove(promo, false);

    EXPECT_NE(board.getHash(), originalHash);

    board.undoMove(undo);

    EXPECT_EQ(board.getHash(), originalHash);
}

TEST_F(ZobristTest, EnPassantTargetAffectsHash) {
    // Same position but one has en passant available
    Board board1("rnbqkbnr/pppp1ppp/8/4pP2/8/8/PPPPP1PP/RNBQKBNR w KQkq e6 0 1");
    Board board2("rnbqkbnr/pppp1ppp/8/4pP2/8/8/PPPPP1PP/RNBQKBNR w KQkq - 0 1");

    EXPECT_NE(board1.getHash(), board2.getHash());
}

TEST_F(ZobristTest, CastlingRightsAffectHash) {
    // Same position but different castling rights
    Board board1("r1bqk2r/pppppppp/8/8/8/8/PPPPPPPP/R1BQK2R w KQkq - 0 1");
    Board board2("r1bqk2r/pppppppp/8/8/8/8/PPPPPPPP/R1BQK2R w Kq - 0 1");

    EXPECT_NE(board1.getHash(), board2.getHash());
}