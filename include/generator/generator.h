#pragma once
#include <vector>

#include "move.h"
#include "board/board.h"

class Generator {
public:
    static std::vector<Move> generatePseudoMoves(Board& board);

    static void generateSlidingCaptures(const Board& board, int r, int c,
                                        std::vector<Move>& captures, Color enemy,
                                        bool diagonal, bool orthogonal);
    static std::vector<Move> generateCaptures(const Board& board);
};
