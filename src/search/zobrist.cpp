#include <search/zobrist.h>


Zobrist::Zobrist() {
    init();
}

uint64_t Zobrist::pieceSquare[2][6][64];
uint64_t Zobrist::sideToMove;
uint64_t Zobrist::castling[16];
uint64_t Zobrist::enPassant[8];

void Zobrist::init() {
    std::mt19937_64 rng(31415926);

    for (auto & color : pieceSquare)
        for (auto & piece : color)
            for (auto & sq : piece)
                sq = rng();

    sideToMove = rng();
    for (auto & i : castling) i = rng();
    for (auto & i : enPassant) i = rng();
}

