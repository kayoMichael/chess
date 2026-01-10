#include "board.h"
#include <iostream>

void Board::init() {
    for (auto& row : board)
        for (auto& cell : row)
            cell = PieceType::EMPTY;

    for (int j = 0; j < 8; j++) {
        board[6][j] = PieceType::W_PAWN;
        board[1][j] = PieceType::B_PAWN;
    }

    PieceType whiteBack[8] = {
        PieceType::W_ROOK, PieceType::W_KNIGHT, PieceType::W_BISHOP, PieceType::W_QUEEN,
        PieceType::W_KING, PieceType::W_BISHOP, PieceType::W_KNIGHT, PieceType::W_ROOK
    };

    PieceType blackBack[8] = {
        PieceType::B_ROOK, PieceType::B_KNIGHT, PieceType::B_BISHOP, PieceType::B_QUEEN,
        PieceType::B_KING, PieceType::B_BISHOP, PieceType::B_KNIGHT, PieceType::B_ROOK
    };

    for (int j = 0; j < 8; j++) {
        board[7][j] = whiteBack[j];
        board[0][j] = blackBack[j];
    }
}

PieceType Board::at(const int r, const int c) const {
    return board[r][c];
}

void Board::print() const {
    std::cout << "  ┌───┬───┬───┬───┬───┬───┬───┬───┐\n";
    for (int row = 0; row < 8; row++) {
        std::cout << 8 - row << " │";
        for (auto & piece : board[row]) {
            std::cout << " ";
            switch (piece) {
                case PieceType::B_ROOK: std::cout << "♜"; break;
                case PieceType::B_KNIGHT: std::cout << "♞"; break;
                case PieceType::B_BISHOP: std::cout << "♝"; break;
                case PieceType::B_QUEEN: std::cout << "♛"; break;
                case PieceType::B_KING: std::cout << "♚"; break;
                case PieceType::B_PAWN: std::cout << "♟"; break;
                case PieceType::EMPTY: std::cout << " "; break;
                case PieceType::W_PAWN: std::cout << "♙"; break;
                case PieceType::W_ROOK: std::cout << "♖"; break;
                case PieceType::W_KNIGHT: std::cout << "♘"; break;
                case PieceType::W_BISHOP: std::cout << "♗"; break;
                case PieceType::W_QUEEN: std::cout << "♕"; break;
                case PieceType::W_KING: std::cout << "♔"; break;
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