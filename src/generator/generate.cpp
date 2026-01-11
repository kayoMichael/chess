#include "generator.h"
#include "pieces.h"
#include "dispatch/piece_dispatch.h"

std::vector<Move> Generator::generateLegalMoves(Board& board) {
    std::vector<Move> moves;
    Color side = board.getColor();

    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            Piece p = board.at(r, c);
            if (p.color == side) {
                dispatchPiece(p.kind).generateMoves(board, r, c, moves);
            }
        }
    }
    std::erase_if(moves, [&](const Move& m) {
        return !board.validate(m);
    });

    return moves;
}
