#pragma once
#include "move.h"
#include "piece_type.h"

class Board {
public:
    Board();
    void init();
    void print() const;
    bool whiteKingMoved = false;
    bool blackKingMoved = false;
    bool whiteRookKingsideMoved = false;
    bool whiteRookQueensideMoved = false;
    bool blackRookKingsideMoved = false;
    bool blackRookQueensideMoved = false;
    MoveUndo makeMove(const Move& move, bool hypothetical);
    void undoMove(const MoveUndo& undo);
    bool validate(const Move& move);
    bool squareAttacked(const Square& square) const;
    [[nodiscard]] bool isChecked() const;
    [[nodiscard]] Color getColor() const;
    [[nodiscard]] Piece at(int r, int c) const;

private:
    Piece board[8][8];
    Color side = Color::White;
    bool canCastleKingside[2]{};
    bool canCastleQueenside[2]{};
    void movePiece(const Square& from, const Square& to);
    void updateCastlingRights(const Piece& piece, const Move& move);
    [[nodiscard]] bool directionalAttacked(Square piece, int dr, int dc) const;
    [[nodiscard]] bool knightAttacked(Square piece) const;
    [[nodiscard]] bool pawnAttacked(Square piece) const;
    [[nodiscard]] bool kingAttacked(Square piece) const;
};