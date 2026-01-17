#pragma once
#include "chess_piece.h"
#include "movement_const.h"
#include "board/board.h"

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

            if (r >= 0 && r < 8 && c >= 0 && c < 8) {
                const auto [kind, color] = board.at(r, c);
                if (dc == 0 && kind != PieceKind::None) continue;
                if (dc != 0 && (pawn.color == color || color == Color::None)) continue;

                if (r == 0 || r == 7) {
                    for (PieceKind promo : {PieceKind::Queen, PieceKind::Rook, PieceKind::Bishop, PieceKind::Knight}) {
                        moves.emplace_back(Square(row, col), Square(r, c), MoveType::Promotion, promo);
                    }
                } else {
                    moves.emplace_back(Square(row, col), Square(r, c));
                }
            }
        }

        if (board.enPassantTarget.has_value()) {
            Square ep = board.enPassantTarget.value();
            // Check if we're adjacent to the en passant target
            if (ep.r == row + direction && std::abs(ep.c - col) == 1) {
                moves.emplace_back(Square(row, col), ep, MoveType::EnPassant);
            }
        }
    }
};
