#include <string>

#include "board/board.h"

int main() {
    const std::string fen = "r3k2r/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w KQkq - 0 1";

    Board board = Board(fen);

    board.print();
}
