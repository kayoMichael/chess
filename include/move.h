#pragma once
#include "piece_type.h"
struct Square {
    int r;
    int c;
    Square() = default;
    Square(int row, int col) : r(row), c(col) {}
};

enum class MoveType { Normal, Castle, EnPassant, Promotion };

struct Move {
    Square current{};
    Square destination{};
    MoveType type = MoveType::Normal;
    PieceKind promotion = PieceKind::None;

    Move() = default;
    Move(const Square c, const Square d)
    : current(c), destination(d) {}
};

struct MoveUndo {
    Move move;
    Piece captured{};
    Piece movedPiece{};
};