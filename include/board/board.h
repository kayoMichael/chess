#pragma once

#include <string>
#include <optional>
#include <vector>
#include <cstdint>

#include "move.h"
#include "piece_type.h"

class Board {
public:
    Board();
    uint64_t hash = 0;
    explicit Board(const std::string& fen);
    void init();
    void print() const;
    void loadFEN(const std::string& fen);
    [[nodiscard]] uint64_t getHash() const { return hash; }
    void computeHash();
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
    [[nodiscard]] std::vector<Piece> pieces() const;
    template <typename Predicate>
    bool rayScan(
        const int startR,
        const int startC,
        const int dr,
        const int dc,
        Predicate pred
    ) const {
        int r = startR + dr;
        int c = startC + dc;

        while (r >= 0 && r < 8 && c >= 0 && c < 8) {
            const auto& p = at(r, c);

            if (pred(p)) return true;

            r += dr;
            c += dc;
        }
        return false;
    };
    template <typename Predicate>
    int rayScanCount(
        const int startR,
        const int startC,
        const int dr,
        const int dc,
        Predicate pred
    ) const {
        int r = startR + dr;
        int c = startC + dc;
        int count = 0;

        while (r >= 0 && r < 8 && c >= 0 && c < 8) {
            const auto& p = at(r, c);

            if (pred(p)) return count;

            r += dr;
            c += dc;
            count++;
        }
        return count;
    };

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