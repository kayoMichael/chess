#include "search/search.h"

#include "board/board.h"
#include "move.h"
#include "generator/generator.h"
#include "piece_type.h"
#include "board/transposition.h"

#include <vector>
#include <algorithm>
#include <cmath>
#include <ranges>


Move Search::findBestMove(Board& board, const int depth) {
    int alpha = -INF;
    int beta = INF;
    rootDepth = depth;
    const std::vector<Move> moves = Generator::generateLegalMoves(board);

    Move bestMove;
    Color side = board.getColor();

    if (side == Color::White) {
        int bestScore = -INF;
        for (auto& move : moves) {
            MoveUndo undo = board.makeMove(move, false);
            int score = alphaBeta(board, depth - 1, 1, alpha, beta);
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
            int score = alphaBeta(board, depth - 1, 1, alpha, beta);
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

int Search::alphaBeta(Board &board, int depth, int ply, int alpha, int beta) {
    const int originalAlpha = alpha;
    const int originalBeta = beta;

    if (depth == 0) { // Don't stop if the board is still violent.
        return quiescence(board, alpha, beta);
    }

    TTEntry* entry = tt.probe(board.getHash());
    if (entry && entry->depth >= depth) {
        if (entry->flag == EXACT) return entry->score;
        if (entry->flag == LOWER_BOUND) alpha = std::max(alpha, entry->score);
        if (entry->flag == UPPER_BOUND) beta  = std::min(beta, entry->score);
        if (alpha >= beta) return entry->score; // or alpha; see note below
    }

    std::vector<Move> moves = Generator::generateLegalMoves(board);
    orderMoves(moves, board);
    const Color side = board.getColor();
    if (moves.empty() && board.isChecked(side)) {
        // Checkmate - side to move loses
        // White checkmated = negative (good for black)
        // Black checkmated = positive (good for white)
        if (side == Color::Black) return MATE - ply;
        return -MATE + ply;
    } else if (moves.empty()) {
        //stalemate
        return 0;
    }

    int bestScore;
    if (side == Color::White) {
        bestScore = -INF;
        for (auto& move : moves) {
            MoveUndo undo = board.makeMove(move, false);
            bool inCheck = board.isChecked(board.getColor());
            int newDepth = depth - 1 + (inCheck ? 1 : 0); // Extend if check
            int score = alphaBeta(board, newDepth, ply + 1, alpha, beta);
            board.undoMove(undo);
            alpha = std::max(alpha, score);
            bestScore = std::max(bestScore, score);
            if (beta <= alpha) break;
        }

        // White is maximizing
        uint8_t flag;
        if (bestScore <= originalAlpha) {
            flag = UPPER_BOUND;  // Never improved alpha, this is the best we can do (or worse)
        } else if (bestScore >= originalBeta) {
            flag = LOWER_BOUND;  // Score is at least this good (caused beta cutoff)
        } else {
           flag = EXACT;
        }
        tt.store(board.getHash(), bestScore, depth, flag);
    } else {
        bestScore = INF;
        for (auto& move : moves) {
            MoveUndo undo = board.makeMove(move,false);
            bool inCheck = board.isChecked(board.getColor());
            int newDepth = depth - 1 + (inCheck ? 1 : 0); // Extend if check
            int score = alphaBeta(board, newDepth, ply + 1, alpha, beta);
            board.undoMove(undo);
            bestScore = std::min(bestScore, score);
            beta = std::min(beta, score);
            if (beta <= alpha) break;
        }
        // Black is minimizing
        uint8_t flag;
        if (bestScore >= originalBeta) {
            flag = LOWER_BOUND;  // Couldn't beat beta, score is at least this
        } else if (bestScore <= originalAlpha) {
            flag = UPPER_BOUND;  // Beat alpha (bad for white), score is at most this
        } else {
            flag = EXACT;
        }
        tt.store(board.getHash(), bestScore, depth, flag);
    }
    return bestScore;
}

int Search::quiescence(Board& board, int alpha, int beta, int qDepth) {
    int stand_pat = evaluate(board);
    Color side = board.getColor();

    if (qDepth >= 8) return stand_pat;

    if (side == Color::White) {
        if (stand_pat >= beta) return beta;
        if (stand_pat > alpha) alpha = stand_pat;
    } else {
        if (stand_pat <= alpha) return alpha;
        if (stand_pat < beta) beta = stand_pat;
    }
    std::vector<Move> moves = Generator::generateLegalMoves(board);
    orderMoves(moves, board);

    int captureCount = 0;
    for (const Move& move : moves) {
        if (board.at(move.destination.r, move.destination.c).kind == PieceKind::None)
            continue;
        captureCount++;

        MoveUndo undo = board.makeMove(move, false);
        int score = quiescence(board, alpha, beta, qDepth + 1);
        board.undoMove(undo);

        if (side == Color::White) {
            if (score > alpha) alpha = score;
            if (alpha >= beta) return beta;
        } else {
            if (score < beta) beta = score;
            if (beta <= alpha) return alpha;
        }
    }

    return side == Color::White ? alpha : beta;
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

// Most Valuable Victim - Least Valuable Attacker. Assumes Only valid moves
int Search::mvvLva(const Move& move, const Board& board) {
    const Piece piece = board.at(move.destination.r, move.destination.c);
    if (piece.kind == PieceKind::None) return 0;

    int victim = pieceValue(piece.kind);
    int attacker = pieceValue(board.at(move.current.r, move.current.c).kind);

    // Higher score = look at first
    // Capturing high value with low value piece = best
    return victim * 10 - attacker;
}

void Search::orderMoves(std::vector<Move>& moves, const Board& board) {
    std::ranges::sort(moves, [&](const Move& a, const Move& b) {
        return mvvLva(a, board) > mvvLva(b, board);
    });
}

int Search::evaluate(const Board &board) {
    int score = 0;

    double phase = computePhase(board) / static_cast<double>(MAX_PHASE);
    phase = std::clamp(phase, 0.0, 1.0);

    auto enemyPieceAheadOnFile = [&](const int r, const int file, const int dr, const int dc, const Color color, const PieceKind piece) {
        return board.rayScan(r, file, dr, dc,
            [&](const Piece& p) {
                return p.kind == piece && p.color != color;
            }
        );
    };

    auto allowedSquares = [&](const int r, const int file, const int dr, const int dc) {
        return board.rayScanCount(r, file, dr, dc,
            [&](const Piece& p) {
                return p.kind != PieceKind::None;
            });
    };

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
            int dr = (color == Color::White) ? -1 : 1;

            switch (kind) {
                case PieceKind::Knight:
                    pstScore = static_cast<int>(std::round(
                        knightPST_EARLY[pst_row][pst_col] * phase +
                        knightPST_LATE[pst_row][pst_col] * (1.0 - phase)
                    ));
                    break;

                case PieceKind::Bishop: {
                    pstScore = static_cast<int>(std::round(
                        bishopPST_EARLY[pst_row][pst_col] * phase +
                        bishopPST_LATE[pst_row][pst_col] * (1.0 - phase)
                    ));
                    const int left_dig = allowedSquares(r, c, dr, -1);
                    const int right_dig = allowedSquares(r, c, dr, 1);
                    const int left_back_dig = allowedSquares(r, c, -dr, -1);
                    const int right_back_dig = allowedSquares(r, c, -dr, 1);

                    pstScore += static_cast<int>(std::round((left_dig + right_dig + left_back_dig * 0.5 + right_back_dig * 0.5) * 2 * phase));
                    break;
                }

                case PieceKind::Rook: {
                    pstScore = static_cast<int>(std::round(
                        rookPST_EARLY[pst_row][pst_col] * phase +
                        rookPST_LATE[pst_row][pst_col] * (1.0 - phase)
                    ));
                    const int forward = allowedSquares(r, c, dr, 0);
                    const int backward = allowedSquares(r, c, -1, 0);
                    const int left = allowedSquares(r, c, 0, -1);
                    const int right = allowedSquares(r, c, 0, 1);
                    pstScore += static_cast<int>(std::round((forward + backward + left * 0.5 + right * 0.5) * 2 * phase));
                    break;
                }

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

                case PieceKind::Pawn: {
                    pstScore = pawnPST[pst_row][pst_col];
                    bool sameFile = enemyPieceAheadOnFile(r, c, dr, 0, color, PieceKind::Pawn);
                    bool leftFile  = ((c > 0) && enemyPieceAheadOnFile(r, c - 1, dr, 0, color, PieceKind::Pawn));
                    bool rightFile = ((c < 7) && enemyPieceAheadOnFile(r, c + 1, dr, 0, color, PieceKind::Pawn));

                    if (!sameFile && !leftFile && !rightFile) {
                        int rankBonus = (color == Color::White) ? (7 - r) : r;  // 0-6, higher = more advanced
                        pstScore += 10 + rankBonus * 15;  // 10 to 100 based on rank
                    }
                    break;
                }
                default:
                    break;
            }

            score += pstScore * sign;
        }
    }

    return score;
}

