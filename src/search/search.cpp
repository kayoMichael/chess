#include "board/board.h"
#include "move.h"
#include "search.h"
#include "generator/generator.h"
#include "piece_type.h"
#include <climits>
#include <algorithm>

Move Search::findBestMove(Board& board, int depth) {
    int alpha = -INT_MAX;
    int beta = INT_MAX;
    std::vector<Move> moves = Generator::generateLegalMoves(board);

    Move bestMove;
    Color side = board.getColor();

    if (side == Color::White) {
        int bestScore = -INT_MAX;
        for (auto& move : moves) {
            MoveUndo undo = board.makeMove(move, false);
            int score = alphaBeta(board, depth - 1, alpha, beta);
            board.undoMove(undo);
            if (score > bestScore) {
                bestScore = score;
                bestMove = move;
            }
            alpha = std::max(alpha, score);
        }
    } else {
        int bestScore = INT_MAX;
        for (auto& move : moves) {
            MoveUndo undo = board.makeMove(move, false);
            int score = alphaBeta(board, depth - 1, alpha, beta);
            board.undoMove(undo);
            if (score < bestScore) {
                bestScore = score;
                bestMove = move;
            }
            beta = std::min(beta, score);
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
    if (side == Color::White) {
        int maxValue = -INT_MAX;
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
    int minValue = INT_MAX;
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
            const Piece p = board.at(r, c);
            if (p.color == Color::White) score += pieceValue(p.kind);
            else score -= pieceValue(p.kind);
        }
    }
    return score;
}
