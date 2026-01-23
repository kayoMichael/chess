#include <iostream>
#include <sstream>
#include <chrono>
#include "board/board.h"
#include "search/search.h"
#include "search/zobrist.h"

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
            int maxDepth = 10;
            int moveTime = 0;
            std::string token;

            while (ss >> token) {
                if (token == "depth") ss >> maxDepth;
                else if (token == "movetime") ss >> moveTime;
            }

            Move bestMove;
            auto searchStart = std::chrono::high_resolution_clock::now();
            for (int depth = 1; depth <= maxDepth; depth++) {
                auto depthStart = std::chrono::high_resolution_clock::now();

                bestMove = search.findBestMove(board, depth);

                auto depthEnd = std::chrono::high_resolution_clock::now();
                auto depthTime = std::chrono::duration_cast<std::chrono::milliseconds>(depthEnd - depthStart).count();
                auto totalTime = std::chrono::duration_cast<std::chrono::milliseconds>(depthEnd - searchStart).count();

                std::cout << "info depth " << depth
                          << " time " << totalTime
                          << " pv " << Board::toUCI(bestMove)
                          << std::endl;

                if (moveTime > 0 && totalTime >= moveTime) {
                    std::cerr << "Time limit reached at depth " << depth << std::endl;
                    break;
                }

                if (depthTime > 8000) {
                    std::cerr << "Depth " << depth << " took " << depthTime << "ms, stopping" << std::endl;
                    break;
                }
            }

            std::cout << "bestmove " << Board::toUCI(bestMove) << "\n";
        }
        else if (cmd == "bench") {
            Board benchBoard;
            search.clearTT();
            search.resetTTStats();

            for (int depth = 1; depth <= 7; depth++) {

                auto depthStart = std::chrono::high_resolution_clock::now();
                Move best = search.findBestMove(benchBoard, depth);
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