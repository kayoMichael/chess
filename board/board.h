#pragma once

enum class PieceType {
    EMPTY,
    W_PAWN, W_KNIGHT, W_BISHOP, W_ROOK, W_QUEEN, W_KING,
    B_PAWN, B_KNIGHT, B_BISHOP, B_ROOK, B_QUEEN, B_KING
};

class Board {
public:
    Board();
    void init();
    void print() const;
    [[nodiscard]] PieceType at(int r, int c) const;

private:
    PieceType board[8][8];
};