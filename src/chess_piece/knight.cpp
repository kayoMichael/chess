#pragma once
#include "chess_piece.h"
#include "board/board.h"
#include "movement_const.h"
#include "move.h"

class Knight : public ChessPiece {
public:
    void generateMoves(Board &board, int row, int col, std::vector<Move> &moves) const override {
        for (const auto [r, c] : MovementConst::KNIGHT_LATTICE_DISPLACEMENTS) {
            int destRow = row + c;
            int destCol = col + r;

            if (0 <= destRow && destRow < 8 && 0 <= destCol && destCol < 8)
                moves.emplace_back(Square(row, col), Square(destRow, destCol));
        }
    }
};