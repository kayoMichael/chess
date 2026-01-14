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

        // castling check
        if (king.color == Color::White && !board.whiteKingMoved && !board.isChecked()) {
            if (!board.whiteRookKingsideMoved) {
                bool canCastle = true;

                for (int c = col + 1; c < 7; c++) {  // squares between king and rook
                    if (board.at(7, c).kind != PieceKind::None) {
                        canCastle = false;
                        break;
                    }
                }

                if (canCastle && !board.squareAttacked(Square(7, 5)) && !board.squareAttacked(Square(7, 6))) {
                    moves.emplace_back(Square(-10, -10), Square(-10, -10));  // kingside castle
                }

            }
            if (!board.whiteRookQueensideMoved) {
                bool canCastle = true;

                for (int c = col - 1; c > 0; c--) {  // squares between king and rook
                    if (board.at(7, c).kind != PieceKind::None) {
                        canCastle = false;
                        break;
                    }
                }
                if (canCastle && !board.squareAttacked(Square(7, 3)) && !board.squareAttacked(Square(7, 2))) {
                    moves.emplace_back(Square(10, 10), Square(10, 10)); // Symbol for Queen Side Castle
                }
            }
        } else if (king.color == Color::Black && !board.blackKingMoved && !board.isChecked()) {
            // Kingside
            if (!board.blackRookKingsideMoved) {
                bool canCastle = true;

                for (int c = col + 1; c < 7; c++) {
                    if (board.at(0, c).kind != PieceKind::None) {
                        canCastle = false;
                        break;
                    }
                }

                if (canCastle && !board.squareAttacked(Square(0, 5)) && !board.squareAttacked(Square(0, 6))) {
                    moves.emplace_back(Square(-10, -10), Square(-10, -10));
                }
            }

            // Queenside
            if (!board.blackRookQueensideMoved) {
                bool canCastle = true;

                for (int c = col - 1; c > 0; c--) {
                    if (board.at(0, c).kind != PieceKind::None) {
                        canCastle = false;
                        break;
                    }
                }

                if (canCastle && !board.squareAttacked(Square(0, 3)) && !board.squareAttacked(Square(0, 2))) {
                    moves.emplace_back(Square(10, 10), Square(10, 10));
                }
            }
        }
    }
};