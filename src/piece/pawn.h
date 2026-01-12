#pragma once
#include "chess_piece.h"
#include "movement_const.h"
#include "pieces.h"
#include "bishop.h"

class Pawn : public ChessPiece {
public:
    void generateMoves(Board &board, int row, int col, std::vector<Move> &moves) const override {
        const Piece pawn = board.at(row, col);
        const int direction = pawn.color == Color::White ? -1 : 1; // Pawn Movement Direction
        const int eligibility = pawn.color == Color::White ? 6 : 1; // Eligibility to Jump twice
        for (const auto [dr, dc] : MovementConst::MINKOWSKI_RESTRICTED_SUM) {
            if (dr == 2 && row != eligibility) continue; // Not on the eligible rank to jump twice
            if (dr == 2 && board.at(row + direction, col).kind != PieceKind::None) continue; // Intercepted by a piece
            const int r = row + dr * direction;
            const int c = (col + dc);

            if (r >= 1 && r < 7 && c >= 0 && c < 8) {
                const auto [kind, color] = board.at(r, c);
                if (dc == 0 && kind != PieceKind::None) continue; // Not capturing but there is a piece in front
                if (dc != 0 && (pawn.color == color || color == Color::None)) continue; // Capturing but the square is occupied by ally or empty

                moves.emplace_back(Square(row, col), Square(r, c));
            }

            if ((pawn.color == Color::White && r == 0) || (pawn.color == Color::Black && r == 7)) {
                for (PieceKind promo : {PieceKind::Queen, PieceKind::Rook, PieceKind::Bishop, PieceKind::Knight}) {
                    moves.emplace_back(Square(row, col), Square(r, c), promo);
                }
            }
        }
    }
};
