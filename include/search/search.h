#pragma once
#include "board/board.h"
#include "move.h"
#include "board/transposition.h"
#include <cstdint>


class Search {
public:
    Search() : tt(64) {};
    void clearTT() { tt.clear(); };
    void resetTTStats() { tt.resetStats(); };
    std::tuple<uint64_t, uint64_t, uint64_t> getTTStats() {
        return {tt.hits, tt.misses, tt.stores};
    };
    int rootDepth{};
    Move findBestMove(Board& board, int depth);
    static int evaluate(const Board& board);
private:
    TranspositionTable tt;
    int currentPST[6][64];  // [pieceKind][square]
    double lastPhase = -1;
    int alphaBeta(Board& board, int depth, int ply, int alpha, int beta);
    static int computePhase(const Board& board);
    static int mvvLva(const Move& move, const Board& board);
    static void orderMoves(std::vector<Move>& moves, const Board& board);
    int quiescence(Board& board, int alpha, int beta, int qDepth = 0);
    void updatePST(double phase);
};