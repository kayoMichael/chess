#include "search/search.h"

#include "board/board.h"
#include "move.h"
#include "generator/generator.h"
#include "piece_type.h"
#include "board/transposition.h"

#include <vector>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <ranges>


Move Search::findBestMove(Board& board, const int depth) {
    int alpha = -INF;
    int beta = INF;
    rootDepth = depth;
    const std::vector<Move> moves = Generator::generatePseudoMoves(board);
    const double phase = computePhase(board) / static_cast<double>(MAX_PHASE);
    updatePST(phase);
    bool foundLegal = false;

    Move bestMove;
    const Color side = board.getColor();

    if (side == Color::White) {
        int bestScore = -INF;
        for (auto& move : moves) {
            MoveUndo undo = board.makeMove(move, false);
            if (board.isChecked(side)) {
                board.undoMove(undo);
                continue;
            }
            foundLegal = true;
            int score = alphaBeta(board, depth - 1, 1, alpha, beta);
            std::cout << Board::toUCI(move) << " score=" << score << "\n";
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
            if (board.isChecked(side)) {
                board.undoMove(undo);
                continue;
            }
            foundLegal = true;
            int score = alphaBeta(board, depth - 1, 1, alpha, beta);
            std::cout << Board::toUCI(move) << " score=" << score << "\n";
            board.undoMove(undo);
            if (score < bestScore) {
                bestScore = score;
                bestMove = move;
            }
            beta = std::min(beta, score);
            if (beta <= alpha) break;
        }
    }

    if (!foundLegal) {
        return Move{};
    }

    return bestMove;
}

int Search::alphaBeta(Board &board, int depth, int ply, int alpha, int beta) {
    const int originalAlpha = alpha;
    const int originalBeta = beta;
    bool foundLegal = false;

    Move ttMove;
    bool hasTTMove = false;

    if (depth == 0) { // Don't stop if the board is still violent.
        return quiescence(board, alpha, beta);
    }
    TTEntry* entry = tt.probe(board.getHash());
    if (entry) {
        ttMove = entry->bestMove;
        hasTTMove = true;

        if (entry->depth >= depth) {
            if (entry->flag == EXACT) return entry->score;
            if (entry->flag == LOWER_BOUND) alpha = std::max(alpha, entry->score);
            if (entry->flag == UPPER_BOUND) beta = std::min(beta, entry->score);
            if (alpha >= beta) return alpha;
        }
    };
    std::vector<Move> moves = Generator::generatePseudoMoves(board);
    orderMoves(moves, board);
    if (hasTTMove) {
        for (size_t i = 0; i < moves.size(); i++) {
            if (moves[i].current.r == ttMove.current.r &&
                moves[i].current.c == ttMove.current.c &&
                moves[i].destination.r == ttMove.destination.r &&
                moves[i].destination.c == ttMove.destination.c) {
                std::swap(moves[0], moves[i]);
                break;
                }
        }
    }
    const Color side = board.getColor();
    int bestScore;
    Move bestMove;
    if (side == Color::White) {
        bestScore = -INF;
        for (auto& move : moves) {
            MoveUndo undo = board.makeMove(move, false);
            if (board.isChecked(side)) {
                board.undoMove(undo);
                continue;
            }
            foundLegal = true;
            int score = alphaBeta(board, depth - 1, ply + 1, alpha, beta);
            board.undoMove(undo);
            alpha = std::max(alpha, score);
            if (score > bestScore) {
                bestScore = score;
                bestMove = move;  // Track best move
            }
            bestScore = std::max(bestScore, score);
            if (beta <= alpha) break;
        }
        if (!foundLegal) {
            if (board.isChecked(side)) {
                return -MATE + ply;
            }
            return 0;
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
        tt.store(board.getHash(), bestScore, depth, flag, bestMove);
    } else {
        bestScore = INF;
        for (auto& move : moves) {
            MoveUndo undo = board.makeMove(move, false);
            if (board.isChecked(side)) {
                board.undoMove(undo);
                continue;
            }
            foundLegal = true;

            int score = alphaBeta(board, depth - 1, ply + 1, alpha, beta);
            board.undoMove(undo);

            if (score < bestScore) {
                bestScore = score;
                bestMove = move;
            }
            beta = std::min(beta, score);
            if (beta <= alpha) break;
        }
        if (!foundLegal) {
            if (board.isChecked(side)) {
                return MATE + ply;
            }
            return 0;
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
        tt.store(board.getHash(), bestScore, depth, flag, bestMove);
    }
    return bestScore;
}

int Search::quiescence(Board& board, int alpha, int beta, int qDepth) {
    int stand_pat = evaluate(board);
    Color side = board.getColor();
    if (qDepth >= 8) return stand_pat;

    if (side == Color::White) {
        if (stand_pat >= beta) return beta;

        int delta = 900;  // Queen value - biggest possible gain
        if (stand_pat + delta < alpha) return alpha;

        if (stand_pat > alpha) alpha = stand_pat;
    } else {
        if (stand_pat <= alpha) return alpha;

        int delta = 900;
        if (stand_pat - delta > beta) return beta;

        if (stand_pat < beta) beta = stand_pat;
    }
    std::vector<Move> caps;
    caps = Generator::generateCaptures(board);
    orderMoves(caps, board);

    for (const Move& move : caps) {
        if (board.at(move.destination.r, move.destination.c).kind == PieceKind::None)
            continue;
        MoveUndo undo = board.makeMove(move, false);
        if (board.isChecked(side)) {
            board.undoMove(undo);
            continue;
        }
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
    std::vector<int> score(moves.size());

    for (size_t i = 0; i < moves.size(); i++) {
        score[i] = mvvLva(moves[i], board);
    }

    std::vector<size_t> idx(moves.size());
    std::iota(idx.begin(), idx.end(), 0);

    std::sort(idx.begin(), idx.end(), [&](size_t i, size_t j) {
        return score[i] > score[j];
    });

    std::vector<Move> ordered;
    ordered.reserve(moves.size());
    for (auto i : idx) ordered.push_back(moves[i]);

    moves.swap(ordered);
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
                    if (color == Color::White) {
                        if (r == 5 && c == 3) { // d3
                            Piece dPawn = board.at(6, 3);  // d2
                            if (dPawn.kind == PieceKind::Pawn && dPawn.color == Color::White) {
                                pstScore -= 30;  // Penalty for blocking d-pawn
                            }
                        } else if (r == 5 && c == 4) { // e3
                            Piece dPawn = board.at(6, 4);  // e2
                            if (dPawn.kind == PieceKind::Pawn && dPawn.color == Color::White) {
                                pstScore -= 30;  // Penalty for blocking e-pawn
                            }
                        }
                    } else {
                        // Black bishop on d6 blocking d7 pawn
                        if (r == 2 && c == 3) {  // d6
                            Piece dPawn = board.at(1, 3);  // d7
                            if (dPawn.kind == PieceKind::Pawn && dPawn.color == Color::Black) {
                                pstScore -= 30;  // Penalty (will become +30 for white after * sign)
                            }
                        }
                        // Black bishop on e6 blocking e7 pawn
                        else if (r == 2 && c == 4) {  // e6
                            Piece ePawn = board.at(1, 4);  // e7
                            if (ePawn.kind == PieceKind::Pawn && ePawn.color == Color::Black) {
                                pstScore -= 30;
                            }
                        }
                    }

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

                case PieceKind::King: {
                    pstScore = static_cast<int>(std::round(
                        kingPST_EARLY[pst_row][pst_col] * phase +
                        kingPST_LATE[pst_row][pst_col] * (1.0 - phase)
                    ));

                    // King safety - only in early/mid game (phase > 0.3)
                    if (phase > 0.3) {
                        if (color == Color::White) {
                            // Kingside castled (king on g1 or h1)
                            if (r == 7 && (c == 6 || c == 7)) {
                                int shield = 0;
                                // Check pawns on f2, g2, h2 xor h3
                                if (board.at(6, 5).kind == PieceKind::Pawn && board.at(6, 5).color == Color::White) shield++;
                                if (board.at(6, 6).kind == PieceKind::Pawn && board.at(6, 6).color == Color::White) shield++;
                                if ((board.at(6, 7).kind == PieceKind::Pawn && board.at(6, 7).color == Color::White) ^
                                    (board.at(5, 7).kind == PieceKind::Pawn && board.at(5, 7).color == Color::White)) shield++;

                                // Bonus for intact shield, penalty for missing pawns
                                pstScore += (shield - 3) * 15;  // -45 if no pawns, 0 if all 3

                                // Extra penalty for open file in front of king
                                if (board.at(6, 6).color != Color::White && board.at(5, 6).color != Color::White) {
                                    pstScore -= 25;  // Open g-file is dangerous
                                }
                            }
                            // Queenside castled (king on c1 or b1)
                            else if (r == 7 && (c == 1 || c == 2)) {
                                int shield = 0;
                                // Check pawns on a2, b2, c2
                                if (board.at(6, 0).kind == PieceKind::Pawn && board.at(6, 0).color == Color::White) shield++;
                                if (board.at(6, 1).kind == PieceKind::Pawn && board.at(6, 1).color == Color::White) shield++;
                                if (board.at(6, 2).kind == PieceKind::Pawn && board.at(6, 2).color == Color::White) shield++;

                                pstScore += (shield - 3) * 15;
                            }
                        } else {
                            // Black kingside castled (king on g8 or h8)
                            if (r == 0 && (c == 6 || c == 7)) {
                                int shield = 0;
                                // Check pawns on f7, g7, h7 xor h6
                                if (board.at(1, 5).kind == PieceKind::Pawn && board.at(1, 5).color == Color::Black) shield++;
                                if (board.at(1, 6).kind == PieceKind::Pawn && board.at(1, 6).color == Color::Black) shield++;
                                if ((board.at(1, 7).kind == PieceKind::Pawn && board.at(1, 7).color == Color::Black) ^
                                    (board.at(2, 7).kind == PieceKind::Pawn && board.at(2, 7).color == Color::Black)) shield++;

                                pstScore += (shield - 3) * 15;

                                // Open g-file penalty
                                if (board.at(1, 6).color != Color::Black && board.at(2, 6).color != Color::Black) {
                                    pstScore -= 25;
                                }
                            }
                            // Black queenside castled (king on c8 or b8)
                            else if (r == 0 && (c == 1 || c == 2)) {
                                int shield = 0;
                                if (board.at(1, 0).kind == PieceKind::Pawn && board.at(1, 0).color == Color::Black) shield++;
                                if (board.at(1, 1).kind == PieceKind::Pawn && board.at(1, 1).color == Color::Black) shield++;
                                if (board.at(1, 2).kind == PieceKind::Pawn && board.at(1, 2).color == Color::Black) shield++;

                                pstScore += (shield - 3) * 15;
                            }
                        }
                    }
                    break;
                }

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

void Search::updatePST(const double phase) {
    if (std::abs(phase - lastPhase) < 0.01) return;  // Skip if phase hasn't changed much
    lastPhase = phase;

    for (int sq = 0; sq < 64; sq++) {
        int r = sq / 8;
        int c = sq % 8;

        currentPST[0][sq] = static_cast<int>(knightPST_EARLY[r][c] * phase + knightPST_LATE[r][c] * (1.0 - phase));
        currentPST[1][sq] = static_cast<int>(bishopPST_EARLY[r][c] * phase + bishopPST_LATE[r][c] * (1.0 - phase));
        currentPST[2][sq] = static_cast<int>(rookPST_EARLY[r][c] * phase + rookPST_LATE[r][c] * (1.0 - phase));
        currentPST[3][sq] = static_cast<int>(queenPST_EARLY[r][c] * phase + queenPST_LATE[r][c] * (1.0 - phase));
        currentPST[4][sq] = static_cast<int>(kingPST_EARLY[r][c] * phase + kingPST_LATE[r][c] * (1.0 - phase));
        currentPST[5][sq] = pawnPST[r][c];
    }
}

