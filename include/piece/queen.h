#pragma once
#include "chess_piece.h"
#include "movement_const.h"

class Queen : public ChessPiece {
public:
    void generateMoves(Board &board, int row, int col, std::vector<Move> &moves) const override {
        Piece queen = board.at(row, col);
        for (const auto [dr, dc] : MovementConst::CHEBYSHEV_DIRECTIONS) {
            int r = row + dr;
            int c = col + dc;
            while (r >= 0 && c >= 0 && r < 8 && c < 8) {
                const auto [kind, color] = board.at(r, c);
                if (color == queen.color) break;
                moves.emplace_back(Square(row, col), Square(r, c));
                if (kind != PieceKind::None) break;
                r += dr;
                c += dc;
            }
        }
    }
};
