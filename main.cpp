#include <iostream>

enum class Piece {
    EMPTY,
    W_PAWN, W_KNIGHT, W_BISHOP, W_ROOK, W_QUEEN, W_KING,
    B_PAWN, B_KNIGHT, B_BISHOP, B_ROOK, B_QUEEN, B_KING
};

Piece board[8][8];

void initBoard() {
    for (auto & i : board)
        for (auto & j : i)
            j = Piece::EMPTY;

    for (int j = 0; j < 8; j++) {
        board[6][j] = Piece::W_PAWN;
        board[1][j] = Piece::B_PAWN;
    }

    Piece whiteBack[8] = {
        Piece::W_ROOK, Piece::W_KNIGHT, Piece::W_BISHOP, Piece::W_QUEEN,
        Piece::W_KING, Piece::W_BISHOP, Piece::W_KNIGHT, Piece::W_ROOK
    };

    Piece blackBack[8] = {
        Piece::B_ROOK, Piece::B_KNIGHT, Piece::B_BISHOP, Piece::B_QUEEN,
        Piece::B_KING, Piece::B_BISHOP, Piece::B_KNIGHT, Piece::B_ROOK
    };

    for (int j = 0; j < 8; j++) {
        board[7][j] = whiteBack[j];
        board[0][j] = blackBack[j];
    }
}

void PrintBoard() {
    std::cout << "  ┌───┬───┬───┬───┬───┬───┬───┬───┐\n";
    for (int row = 0; row < 8; row++) {
        std::cout << 8 - row << " │";
        for (auto & piece : board[row]) {
            std::cout << " ";
            switch (piece) {
                case Piece::B_ROOK: std::cout << "♜"; break;
                case Piece::B_KNIGHT: std::cout << "♞"; break;
                case Piece::B_BISHOP: std::cout << "♝"; break;
                case Piece::B_QUEEN: std::cout << "♛"; break;
                case Piece::B_KING: std::cout << "♚"; break;
                case Piece::B_PAWN: std::cout << "♟"; break;
                case Piece::EMPTY: std::cout << " "; break;
                case Piece::W_PAWN: std::cout << "♙"; break;
                case Piece::W_ROOK: std::cout << "♖"; break;
                case Piece::W_KNIGHT: std::cout << "♘"; break;
                case Piece::W_BISHOP: std::cout << "♗"; break;
                case Piece::W_QUEEN: std::cout << "♕"; break;
                case Piece::W_KING: std::cout << "♔"; break;
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

int main() {
    initBoard();
    PrintBoard();
    return 0;
}
