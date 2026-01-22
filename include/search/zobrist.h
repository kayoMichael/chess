#pragma once

#include <random>
#include "pieces.h"

class Zobrist {
public:
    Zobrist();

    static void init();
    static uint64_t pieceSquare[2][6][64];  // [color][piece][square]
    static uint64_t sideToMove;
    static uint64_t castling[16]; // castling rights as a 4-bit bitmask
    static uint64_t enPassant[8]; // enpassant rights as a 2-bit bitmask

    static int colorIndex(Color c) { return c == Color::White ? 0 : 1; }

    static int pieceIndex(PieceKind k) {
        switch (k) {
            case PieceKind::Pawn:   return 0;
            case PieceKind::Knight: return 1;
            case PieceKind::Bishop: return 2;
            case PieceKind::Rook:   return 3;
            case PieceKind::Queen:  return 4;
            case PieceKind::King:   return 5;
            default: return 0;
        }
    }

    static int squareIndex(int r, int c) { return r * 8 + c; }
};