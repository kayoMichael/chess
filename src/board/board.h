#pragma once
#include "move.h"
#include "piece_type.h"

class Board {
public:
    Board();
    void init();
    void print() const;
    MoveUndo makeMove(const Move& move, bool hypothetical);
    void undoMove(const MoveUndo& undo);
    bool validate(const Move& move);
    [[nodiscard]] bool isChecked() const;
    [[nodiscard]] Color getColor() const;
    [[nodiscard]] Piece at(int r, int c) const;

private:
    Piece board[8][8];
    Color side = Color::White;
    [[nodiscard]] bool directionalAttacked(Square piece, int dr, int dc) const;
    [[nodiscard]] bool knightAttacked(Square piece) const;
    [[nodiscard]] bool pawnAttacked(Square piece) const;
    [[nodiscard]] bool kingAttacked(Square piece) const;
};