#pragma once
#include <vector>
#include "board/board.h"
#include "move.h"

class ChessPiece {
public:
    virtual ~ChessPiece() = default;
    virtual void generateMoves(Board& board, int row, int col, std::vector<Move>& moves) const = 0;
};
