#pragma once
#include <vector>

#include "move.h"
#include "board/board.h"

class Generator {
public:
    static std::vector<Move> generateLegalMoves(Board& board) ;
};
