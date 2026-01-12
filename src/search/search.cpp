#include "board/board.h"
#include "move.h"
#include "search.h"
#include "generator/generator.h"
#include "piece_type.h"
#include <algorithm>

Move Search::findBestMove(Board& board, int depth) {
    int alpha = -INF;
    int beta = INF;
    std::vector<Move> moves = Generator::generateLegalMoves(board);

    Move bestMove;
    Color side = board.getColor();

    if (side == Color::White) {
        int bestScore = -INF;
        for (auto& move : moves) {
            MoveUndo undo = board.makeMove(move, false);
            int score = alphaBeta(board, depth - 1, alpha, beta);
            board.undoMove(undo);
            if (score > bestScore) {
                bestScore = score;
                bestMove = move;
            }
            alpha = std::max(alpha, score);
            if (beta <= alpha) break;
        }
    } else {
        int bestScore = INF;
        for (auto& move : moves) {
            MoveUndo undo = board.makeMove(move, false);
            int score = alphaBeta(board, depth - 1, alpha, beta);
            board.undoMove(undo);
            if (score < bestScore) {
                bestScore = score;
                bestMove = move;
            }
            beta = std::min(beta, score);
            if (beta <= alpha) break;
        }
    }

    return bestMove;
}

int Search::alphaBeta(Board &board, int depth, int alpha, int beta) {
    if (depth == 0) {
        return evaluate(board);
    }
    std::vector<Move> moves = Generator::generateLegalMoves(board);
    Color side = board.getColor();
    if (moves.empty() && board.isChecked()) {
        // checkmate
        if (side == Color::Black) return MATE - depth; // Find shortest checkmate
        return -MATE + depth;
    } else if (moves.empty()) {
        //stalemate
        return 0;
    }

    if (side == Color::White) {
        int maxValue = -INF;
        for (auto& move : moves) {
            MoveUndo undo = board.makeMove(move, false);
            int score = alphaBeta(board, depth - 1, alpha, beta);
            board.undoMove(undo);
            alpha = std::max(alpha, score);
            maxValue = std::max(maxValue, score);
            if (beta <= alpha) break;
        }
        return maxValue;
    }
    int minValue = INF;
    for (auto& move : moves) {
        MoveUndo undo = board.makeMove(move,false);
        int score = alphaBeta(board, depth - 1, alpha, beta);
        board.undoMove(undo);
        minValue = std::min(minValue, score);
        beta = std::min(beta, score);
        if (beta <= alpha) break;
    }
    return minValue;
}

int Search::evaluate(const Board &board) {
    int score = 0;
    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            const auto [kind, color] = board.at(r, c);
            if (color == Color::White) score += pieceValue(kind);
            else score -= pieceValue(kind);
        }
    }
    return score;
}
