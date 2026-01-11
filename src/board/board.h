#pragma once
#include "move.h"

enum class Color { None, White, Black };
enum class PieceKind { None, Pawn, Knight, Bishop, Rook, Queen, King };

struct Piece {
    PieceKind kind;
    Color color;
};

class Board {
public:
    Board();
    void init();
    void print() const;
    void makeMove(const Move& move, bool hypothetical);
    bool validate(const Move& move);
    [[nodiscard]] Piece at(int r, int c) const;

private:
    Piece board[8][8];
    Color side = Color::White;
    [[nodiscard]] bool directionalAttacked(Square piece, int dr, int dc) const;
    [[nodiscard]] bool knightAttacked(Square piece) const;
    [[nodiscard]] bool pawnAttacked(Square piece) const;
    [[nodiscard]] bool kingAttacked(Square piece) const;
};