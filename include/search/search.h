#pragma once
#include "board/board.h"
#include "move.h"
#include "board/transposition.h"

class Search {
public:
    Search() : tt(64) {}
    Move findBestMove(Board& board, int depth);
    static int evaluate(const Board& board);
private:
    TranspositionTable tt;
    int alphaBeta(Board& board, int depth, int ply, int alpha, int beta);
    static int computePhase(const Board& board);
    static int mvvLva(const Move& move, const Board& board);
    static void orderMoves(std::vector<Move>& moves, const Board& board);
    static int quiescence(Board& board, int alpha, int beta, int qDepth = 0);
};