#pragma once
#include "piece_type.h"
struct Square {
    int r;
    int c;
};

enum class MoveType { Normal, Castle, EnPassant, Promotion };

struct Move {
    Square current{};
    Square destination{};
    MoveType type = MoveType::Normal;
    PieceKind promotion = PieceKind::None;
};

struct MoveUndo {
    Move move;
    Piece captured{};
    Piece movedPiece{};
};