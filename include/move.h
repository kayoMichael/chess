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
    Move(const Square c, const Square d, const MoveType t = MoveType::Normal, const PieceKind p = PieceKind::None)
    : current(c), destination(d), promotion(p), type(t) {}
};

struct MoveUndo {
    Move move;
    Piece captured{};
    Piece movedPiece{};

    bool whiteKingMoved{};
    bool blackKingMoved{};
    bool whiteRookKingsideMoved{};
    bool whiteRookQueensideMoved{};
    bool blackRookKingsideMoved{};
    bool blackRookQueensideMoved{};
};