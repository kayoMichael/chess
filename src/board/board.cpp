#include <iostream>
#include <cassert>

#include "board.h"
#import "move.h"
#import "movement_const.h"

Board::Board() : board{} {
    init();
}

void Board::init() {
    for (auto& row : board)
        for (auto& cell : row)
            cell = Piece(PieceKind::None, Color::None);

    for (int j = 0; j < 8; j++) {
        board[6][j] = Piece(PieceKind::Pawn, Color::White);
        board[1][j] = Piece(PieceKind::Pawn, Color::Black);
    }

    Piece whiteBack[8] = {
        Piece(PieceKind::Rook, Color::White), Piece(PieceKind::Knight, Color::White), Piece(PieceKind::Bishop, Color::White), Piece(PieceKind::Queen, Color::White),
        Piece(PieceKind::King, Color::White), Piece(PieceKind::Bishop, Color::White), Piece(PieceKind::Knight, Color::White), Piece(PieceKind::Rook, Color::White)
    };

    Piece blackBack[8] = {
        Piece(PieceKind::Rook, Color::Black), Piece(PieceKind::Knight, Color::Black), Piece(PieceKind::Bishop, Color::Black), Piece(PieceKind::Queen, Color::Black),
        Piece(PieceKind::King, Color::Black), Piece(PieceKind::Bishop, Color::Black), Piece(PieceKind::Knight, Color::Black), Piece(PieceKind::Rook, Color::Black)
    };

    for (int j = 0; j < 8; j++) {
        board[7][j] = whiteBack[j];
        board[0][j] = blackBack[j];
    }
}

Piece Board::at(const int r, const int c) const {
    assert(r >= 0 && r < 8 && c >= 0 && c < 8);
    return board[r][c];
}

bool Board::validate(const Move& move) {
    // Check if the square is already occupied by an ally
    Piece dest = at(move.destination.r, move.destination.c);
    if (dest.color == side)
        return false;

    Piece src = at(move.current.r, move.current.c);
    // Cannot move empty square
    if (src.kind == PieceKind::None)
        return false;

    // Cannot move opponent’s piece
    if (src.color != side)
        return false;

    // Hypothetically make the move and check
    Board hypothetical = *this;
    hypothetical.makeMove(move, true);
    Square kingPosition = {};
    bool found = false;
    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            Piece piece = hypothetical.at(r, c);
            if (piece.color == hypothetical.side && piece.kind == PieceKind::King) {
                kingPosition.r = r;
                kingPosition.c = c;
                found = true;
                break;
            }
        }
        if (found) break;
    }

    // Pawn Attack
    if (hypothetical.pawnAttacked(kingPosition)) return false;

    // Knight Attack
    if (hypothetical.knightAttacked(kingPosition)) return false;

    // King Attack
    if (hypothetical.kingAttacked(kingPosition)) return false;

    for (auto [r, c]: MovementConst::CHEBYSHEV_DIRECTIONS) {
        if (hypothetical.directionalAttacked(kingPosition, r, c)) return false;
    }

    return true;
}

bool Board::pawnAttacked(Square piece) const {
    int dir = (side == Color::White) ? -1 : 1;
    for (int dc : {-1, 1}) {
        const int nr = piece.r + dir;
        const int nc = piece.c + dc;
        if (0 > nr || nr >= 8 || 0 > nc || nc >= 8) continue;

        Piece p = at(nr, nc);
        if (p.color != side && p.kind == PieceKind::Pawn)
            return true;
    }
    return false;
}

bool Board::directionalAttacked(Square piece, int dr, int dc) const{
    int r = piece.r + dr;
    int c = piece.c + dc;

    while (r >= 0 && r < 8 && c >= 0 && c < 8) {
        Piece p = at(r, c);

        if (p.color == Color::None) {
            r += dr;
            c += dc;
            continue;
        }
        if (p.color != side) {
            if ((p.kind == PieceKind::Rook || p.kind == PieceKind::Queen) &&
                (dr == 0 || dc == 0)) {
                return true;
                }

            if ((p.kind == PieceKind::Bishop || p.kind == PieceKind::Queen) &&
                (dr != 0 && dc != 0)) {
                return true;
                }
            return false; // Enemy piece blocking the potential Attacker
        }
        return false; // Our own piece blocks the potential attacker
    }

    return false;
}

bool Board::knightAttacked(Square piece) const {
    for (auto [dr, dc] : MovementConst::KNIGHT_LATTICE_DISPLACEMENTS) {
        int nr = dr + piece.r;
        int nc = dc + piece.c;
        if (0 <= nr && nr < 8 && 0 <= nc && nc < 8) {
            Piece potentialKnight = at(nr, nc);
            if (side != potentialKnight.color && potentialKnight.kind == PieceKind::Knight) {
                return true;
            }
        }
    }
    return false;
}

bool Board::kingAttacked(Square piece) const {
    for (auto [r, c]: MovementConst::CHEBYSHEV_DIRECTIONS) {
        int nr = piece.r + r;
        int nc = piece.c + c;

        if (0 <= nr && nr < 8 && 0 <= nc && nc < 8) {
            Piece p = at(nr, nc);
            if (p.kind == PieceKind::King) {
                return true;
            }
        }
    }
    return false;
}

void Board::makeMove(const Move& move, bool hypothetical) {
    Piece current_piece = board[move.current.r][move.current.c];
    board[move.current.r][move.current.c] = Piece(PieceKind::None, Color::None);
    board[move.destination.r][move.destination.c] = current_piece;

    if (!hypothetical) {
        side = (side == Color::White) ? Color::Black : Color::White;
    }
}

void Board::print() const {
    std::cout << "  ┌───┬───┬───┬───┬───┬───┬───┬───┐\n";
    for (int row = 0; row < 8; row++) {
        std::cout << 8 - row << " │";
        for (int col = 0; col < 8; col++) {
            Piece p = board[row][col];
            std::cout << " ";
            if (p.kind == PieceKind::None) std::cout << " ";
            else if (p.color == Color::White) {
                switch (p.kind) {
                    case PieceKind::Pawn:   std::cout << "♙"; break;
                    case PieceKind::Knight: std::cout << "♘"; break;
                    case PieceKind::Bishop: std::cout << "♗"; break;
                    case PieceKind::Rook:   std::cout << "♖"; break;
                    case PieceKind::Queen:  std::cout << "♕"; break;
                    case PieceKind::King:   std::cout << "♔"; break;
                    default: break;
                }
            } else {
                switch (p.kind) {
                    case PieceKind::Pawn:   std::cout << "♟"; break;
                    case PieceKind::Knight: std::cout << "♞"; break;
                    case PieceKind::Bishop: std::cout << "♝"; break;
                    case PieceKind::Rook:   std::cout << "♜"; break;
                    case PieceKind::Queen:  std::cout << "♛"; break;
                    case PieceKind::King:   std::cout << "♚"; break;
                    default: break;
                }
            }
            std::cout << " │";
        }
        std::cout << "\n";
        if (row < 7)
            std::cout << "  ├───┼───┼───┼───┼───┼───┼───┼───┤\n";
    }
    std::cout << "  └───┴───┴───┴───┴───┴───┴───┴───┘\n";
    std::cout << "    a   b   c   d   e   f   g   h\n";
}