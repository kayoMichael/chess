#pragma once
#include "chess_piece.h"
#include "movement_const.h"

class Rook : public ChessPiece {
public:
    void generateMoves(Board &board, int row, int col, std::vector<Move> &moves) const override {
        const Piece rook = board.at(row, col);
        for (const auto [dr, dc] : MovementConst::MANHATTAN_BASIS_VECTORS) {
            int r = row + dr;
            int c = col + dc;

            while (r >= 0 && r < 8 && c >= 0 && c < 8) {
                const auto [kind, color] = board.at(r, c);
                if (color == rook.color) break;

                moves.emplace_back(Square(row, col), Square(r, c));

                if (kind != PieceKind::None) break;
            }
        }
    }
};
