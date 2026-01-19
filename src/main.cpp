#include <iostream>
#include <sstream>
#include "board/board.h"
#include "search/search.h"

void uciLoop() {
    Board board;
    Search search;
    std::string line;

    while (std::getline(std::cin, line)) {
        std::istringstream ss(line);
        std::string cmd;
        ss >> cmd;

        if (cmd == "uci") {
            std::cout << "id name Viktoriya Ivanovna Serebryakova\n";
            std::cout << "id author Michael Li\n";
            std::cout << "uciok\n";
        }
        else if (cmd == "isready") {
            std::cout << "readyok\n";
        }
        else if (cmd == "ucinewgame") {
            board = Board();
        }
        else if (cmd == "position") {
            std::string token;
            ss >> token;

            if (token == "startpos") {
                board = Board();
            } else if (token == "fen") {
                std::string fen;
                for (int i = 0; i < 6 && ss >> token; i++) {
                    fen += (i > 0 ? " " : "") + token;
                }
                board = Board(fen);
            }

            // Parse moves
            while (ss >> token) {
                if (token == "moves") continue;
                auto move = board.parseUCI(token);
                if (move) board.makeMove(*move, false);
            }
        }
        else if (cmd == "go") {
            // Simple: fixed depth
            int depth = 5;
            std::string token;
            while (ss >> token) {
                if (token == "depth") ss >> depth;
            }

            Move best = search.findBestMove(board, depth);
            std::cout << "bestmove " << Board::toUCI(best) << "\n";
        }
        else if (cmd == "quit") {
            break;
        }

        std::cout.flush();
    }
}

int main() {
    uciLoop();
    return 0;
}