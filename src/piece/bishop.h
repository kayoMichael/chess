#pragma once
#include "chess_piece.h"
#include "movement_const.h"

class Bishop : public ChessPiece {
public:
    void generateMoves(Board &board, int row, int col, std::vector<Move> &moves) const override {
        Piece bishop = board.at(row, col);

        for (const auto [dr, dc] : MovementConst::DIAGONAL_LATTICE_DIRECTIONS) {
            int r = row + dr;
            int c = col + dc;

            while (0 <= r && r < 8 && 0 <= c && c < 8) {
                const auto [kind, color] = board.at(r, c);
                if (bishop.color == color) break;

                moves.emplace_back(Square(row, col), Square(r, c));

                if (kind != PieceKind::None) break;
                r += dr;
                c += dc;
            }
        }
    }
};
