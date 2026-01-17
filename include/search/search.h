#pragma once
#include "board/board.h"
#include "move.h"

class Search {
public:
    Move findBestMove(Board& board, int depth);
private:
    int alphaBeta(Board& board, int depth, int alpha, int beta);
    static int evaluate(const Board& board);
};