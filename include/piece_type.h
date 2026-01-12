#pragma once

enum class Color { None, White, Black };
enum class PieceKind { None, Pawn, Knight, Bishop, Rook, Queen, King };

constexpr int pieceValue(PieceKind kind) {
    switch (kind) {
        case PieceKind::None:   return 0;
        case PieceKind::Pawn:   return 100;
        case PieceKind::Knight: return 320;
        case PieceKind::Bishop: return 330;
        case PieceKind::Rook:   return 500;
        case PieceKind::Queen:  return 900;
        case PieceKind::King:   return 20000;
        default: return 0;
    }
}

struct Piece {
    PieceKind kind;
    Color color;
};