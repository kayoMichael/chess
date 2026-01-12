#pragma once
#include "chess_piece.h"
#include "movement_const.h"

class King : public ChessPiece {
public:
    void generateMoves(Board &board, int row, int col, std::vector<Move> &moves) const override {
        Piece king = board.at(row, col);
        for (const auto [dr, dc] : MovementConst::CHEBYSHEV_DIRECTIONS) {
            const int r = row + dr;
            const int c = col + dc;
            if (r >= 0 && r < 8 && c >= 0 && c < 8) {
                const auto [kind, color] = board.at(r, c);
                if (color == king.color) continue;
                moves.emplace_back(Square(row, col), Square(r, c));
            }
        }
    }
};
