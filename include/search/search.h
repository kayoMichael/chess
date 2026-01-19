#pragma once
#include "board/board.h"
#include "move.h"

class Search {
public:
    Move findBestMove(Board& board, int depth);
    int evaluate(const Board& board);
private:
    int alphaBeta(Board& board, int depth, int alpha, int beta);
    static int computePhase(const Board& board);
};