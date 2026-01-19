#pragma once

#include <string>
#include <optional>

#include "move.h"
#include "piece_type.h"

class Board {
public:
    Board();
    explicit Board(const std::string& fen);
    void init();
    void print() const;
    void loadFEN(const std::string& fen);
    [[nodiscard]] std::string toFEN() const;
    bool whiteKingMoved = false;
    bool blackKingMoved = false;
    bool whiteRookKingsideMoved = false;
    bool whiteRookQueensideMoved = false;
    bool blackRookKingsideMoved = false;
    bool blackRookQueensideMoved = false;
    std::optional<Square> enPassantTarget = std::nullopt;
    MoveUndo makeMove(const Move& move, bool hypothetical);
    void undoMove(const MoveUndo& undo);
    bool validate(const Move& move);
    [[nodiscard]] bool squareAttacked(const Square& square, Color attackerColor) const;
    [[nodiscard]] bool isChecked(Color kingColor) const;
    [[nodiscard]] Color getColor() const;
    [[nodiscard]] Piece at(int r, int c) const;
    [[nodiscard]] std::optional<Move> parseUCI(const std::string& uci) const;
    static std::string toUCI(const Move& move);

private:
    Piece board[8][8]{};
    Color side = Color::White;
    void movePiece(const Square& from, const Square& to);
    void updateCastlingRights(const Piece& piece, const Move& move);
    void setAt(int r, int c, Piece p);
    void setSide(Color c);
    static PieceKind charToKind(char c);
    static char kindToChar(PieceKind kind, Color color);
    [[nodiscard]] bool directionalAttacked(Square piece, int dr, int dc, Color attackerColor) const;
    [[nodiscard]] bool knightAttacked(Square piece, Color attackerColor) const;
    [[nodiscard]] bool pawnAttacked(Square piece, Color attackerColor) const;
    [[nodiscard]] bool kingAttacked(Square piece, Color attackerColor) const;
};