#pragma once
#include <vector>
#include "board/board.h"
#include "move/move.h"

class Piece {
public:
    virtual ~Piece() = default;
    virtual void generateMoves(Board& board, int row, int col, std::vector<Move>& moves) const = 0;
};
