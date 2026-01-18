#pragma once
#include "chess_piece.h"
#include "movement_const.h"

class King : public ChessPiece {
public:
    void generateMoves(Board &board, int row, int col, std::vector<Move> &moves) const override {
        const Piece king = board.at(row, col);
        const Color opponent = (king.color == Color::White) ? Color::Black : Color::White;
        for (const auto [dr, dc] : MovementConst::CHEBYSHEV_DIRECTIONS) {
            const int r = row + dr;
            const int c = col + dc;
            if (r >= 0 && r < 8 && c >= 0 && c < 8) {
                const auto [kind, color] = board.at(r, c);
                if (color == king.color) continue;
                moves.emplace_back(Square(row, col), Square(r, c));
            }
        }

        // castling
        // king must be on starting square (e1 or e8). Only applicable for FEN since moving king auto loses eligibility.
        if (col != 4) return;
        if (king.color == Color::White && !board.whiteKingMoved && !board.isChecked(Color::White)) {
            tryAddCastle(board, row, 7, col, 1, board.whiteRookKingsideMoved, moves, opponent);   // kingside
            tryAddCastle(board, row, 0, col, -1, board.whiteRookQueensideMoved, moves, opponent); // queenside
        } else if (king.color == Color::Black && !board.blackKingMoved && !board.isChecked(Color::Black)) {
            tryAddCastle(board, row, 7, col, 1, board.blackRookKingsideMoved, moves, opponent);   // kingside
            tryAddCastle(board, row, 0, col, -1, board.blackRookQueensideMoved, moves, opponent); // queenside
        }
    }

private:
    static void tryAddCastle(Board &board, int row, int rookCol, int kingCol,
                             int dir, bool rookMoved, std::vector<Move> &moves, Color opponent) {
        if (rookMoved) return;

        for (int c = kingCol + dir; c != rookCol; c += dir) {
            if (board.at(row, c).kind != PieceKind::None) return;
        }

        int dest = kingCol + 2 * dir;
        if (board.squareAttacked(Square(row, kingCol + dir), opponent) ||
            board.squareAttacked(Square(row, dest), opponent)) {
            return;
            }

        moves.emplace_back(Square(row, kingCol), Square(row, dest), MoveType::Castle);
    }
};