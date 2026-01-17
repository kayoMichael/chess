#include "dispatch/piece_dispatch.h"
#include <unordered_map>

const ChessPiece& dispatchPiece(PieceKind kind) {
    static const Pawn pawn;
    static const Knight knight;
    static const Bishop bishop;
    static const Rook rook;
    static const Queen queen;
    static const King king;

    static const std::unordered_map<PieceKind, const ChessPiece*> dispatch = {
        {PieceKind::Pawn, &pawn},
        {PieceKind::Knight, &knight},
        {PieceKind::Bishop, &bishop},
        {PieceKind::Rook, &rook},
        {PieceKind::Queen, &queen},
        {PieceKind::King, &king}
    };

    return *dispatch.at(kind);
}