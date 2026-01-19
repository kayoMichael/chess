#include "search/search.h"

#include "board/board.h"
#include "move.h"
#include "generator/generator.h"
#include "piece_type.h"
#include <algorithm>
#include <sys/stat.h>

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
    const Color side = board.getColor();
    if (moves.empty() && board.isChecked(side)) {
        // Checkmate - side to move loses
        // White checkmated = negative (good for black)
        // Black checkmated = positive (good for white)
        if (side == Color::Black) return MATE - depth;
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

int Search::computePhase(const Board& board) {
    int phase = 0;

    for (auto& piece : board.pieces()) {
        switch (piece.kind) {
            case PieceKind::Knight: phase += PHASE_KNIGHT; break;
            case PieceKind::Bishop: phase += PHASE_BISHOP; break;
            case PieceKind::Rook: phase += PHASE_ROOK; break;
            case PieceKind::Queen: phase += PHASE_QUEEN; break;
            default: break;
        }
    }

    return phase;
}

int Search::evaluate(const Board &board) {
    int score = 0;

    double phase = computePhase(board) / static_cast<double>(MAX_PHASE);
    phase = std::clamp(phase, 0.0, 1.0);

    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            const auto [kind, color] = board.at(r, c);
            if (kind == PieceKind::None) continue;

            const int sign = (color == Color::White) ? 1 : -1;

            // material
            score += pieceValue(kind) * sign;

            // PST coordinates
            int pst_row = (color == Color::White) ? r : 7 - r; // reverse for black
            int pst_col = c;

            int pstScore = 0;

            switch (kind) {
                case PieceKind::Knight:
                    pstScore = static_cast<int>(std::round(
                        knightPST_EARLY[pst_row][pst_col] * phase +
                        knightPST_LATE[pst_row][pst_col] * (1.0 - phase)
                    ));
                    break;

                case PieceKind::Bishop:
                    pstScore = static_cast<int>(std::round(
                        bishopPST_EARLY[pst_row][pst_col] * phase +
                        bishopPST_LATE[pst_row][pst_col] * (1.0 - phase)
                    ));
                    break;

                case PieceKind::Rook:
                    pstScore = static_cast<int>(std::round(
                        rookPST_EARLY[pst_row][pst_col] * phase +
                        rookPST_LATE[pst_row][pst_col] * (1.0 - phase)
                    ));
                    break;

                case PieceKind::Queen:
                    pstScore = static_cast<int>(std::round(
                        queenPST_EARLY[pst_row][pst_col] * phase +
                        queenPST_LATE[pst_row][pst_col] * (1.0 - phase)
                    ));
                    break;

                case PieceKind::King:
                    pstScore = static_cast<int>(std::round(
                        kingPST_EARLY[pst_row][pst_col] * phase +
                        kingPST_LATE[pst_row][pst_col] * (1.0 - phase)
                    ));

                    break;

                case PieceKind::Pawn:
                    pstScore = pawnPST[pst_row][pst_col];
                    break;

                default:
                    break;
            }

            score += pstScore * sign;
        }
    }

    return score;
}

