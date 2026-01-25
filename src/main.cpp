#include <iostream>
#include <sstream>
#include <chrono>
#include "board/board.h"
#include "search/search.h"
#include "search/zobrist.h"
#include "profiler.h"

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
            search.clearTT();
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
            int depth = 5;
            std::string token;
            while (ss >> token) {
                if (token == "depth") ss >> depth;
            }

            Move best = search.findBestMove(board, depth);
            std::cout << "bestmove " << Board::toUCI(best) << "\n";
        }
        else if (cmd == "bench") {
            Board benchBoard;
            search.clearTT();
            search.resetTTStats();

            for (int depth = 1; depth <= 7; depth++) {

                auto depthStart = std::chrono::high_resolution_clock::now();
                Profiler::reset();
                Move best = search.findBestMove(benchBoard, depth);
                Profiler::print();
                auto depthEnd = std::chrono::high_resolution_clock::now();
                auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(depthEnd - depthStart).count();

                auto [hits, misses, stores] = search.getTTStats();
                double hitRate = (hits + misses) > 0 ? (100.0 * hits / (hits + misses)) : 0;

                std::cout << "Depth " << depth << ": " << ms << "ms"
                          << ", best=" << Board::toUCI(best)
                          << ", TT hits=" << hits << " misses=" << misses
                          << " (" << hitRate << "%)"
                          << ", stores=" << stores << "\n";
            }
        }
        else if (cmd == "quit") {
            break;
        }

        std::cout.flush();
    }
}

int main() {
    Zobrist::init();
    uciLoop();
    return 0;
}