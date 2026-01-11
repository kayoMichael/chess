#include "chess_piece.h"
#include "board/board.h"
#include "movement_const.h"
#include "move.h"

class Knight : public ChessPiece {
public:
    void generateMoves(Board &board, int row, int col, std::vector<Move> &moves) const override {
        for (const auto [r, c] : MovementConst::KNIGHT_LATTICE_DISPLACEMENTS) {
            Move hypotheticalMove = Move(Square(row, col), Square(row + r, row + c));
            if (board.validate(hypotheticalMove)) moves.push_back(hypotheticalMove);
        }
    }
};