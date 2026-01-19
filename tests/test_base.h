#pragma once
#include <gtest/gtest.h>
#include <board/board.h>
#include <move.h>

template <typename PieceT>
class TestBase : public ::testing::Test {
protected:
    PieceT piece;
    std::vector<Move> moves;

    void generateMoves(Board& board, int row, int col) {
        moves.clear();
        piece.generateMoves(board, row, col, moves);
    }

    [[nodiscard]] bool hasMove(int toR, int toC) const {
        for (const auto& m : moves) {
            if (m.destination.r == toR && m.destination.c == toC)
                return true;
        }
        return false;
    }
};
