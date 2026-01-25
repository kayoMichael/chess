#include "generator/generator.h"
#include "pieces.h"
#include "dispatch/piece_dispatch.h"

std::vector<Move> Generator::generatePseudoMoves(Board& board) {
    std::vector<Move> moves;
    Color side = board.getColor();

    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            Piece p = board.at(r, c);
            if (p.color == side) {
                dispatchPiece(p.kind).generateMoves(board, r, c, moves);
            }
        }
    }
    return moves;
}

void Generator::generateSlidingCaptures(const Board& board, int r, int c,
                                         std::vector<Move>& captures, Color enemy,
                                         bool diagonal, bool orthogonal) {
    const int dirs[8][2] = {{-1,-1},{-1,0},{-1,1},{0,-1},{0,1},{1,-1},{1,0},{1,1}};

    for (int i = 0; i < 8; i++) {
        int dr = dirs[i][0], dc = dirs[i][1];
        bool isDiag = (dr != 0 && dc != 0);
        if (isDiag && !diagonal) continue;
        if (!isDiag && !orthogonal) continue;

        int nr = r + dr, nc = c + dc;
        while (nr >= 0 && nr <= 7 && nc >= 0 && nc <= 7) {
            Piece target = board.at(nr, nc);
            if (target.kind != PieceKind::None) {
                if (target.color == enemy) {
                    captures.push_back(Move({r, c}, {nr, nc}));
                }
                break;  // Blocked
            }
            nr += dr;
            nc += dc;
        }
    }
}

std::vector<Move> Generator::generateCaptures(const Board& board) {
    std::vector<Move> captures;
    captures.reserve(32);

    const Color side = board.getColor();
    const Color enemy = (side == Color::White) ? Color::Black : Color::White;

    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            const Piece piece = board.at(r, c);
            if (piece.color != side) continue;

            switch (piece.kind) {
                case PieceKind::Pawn: {
                    int dir = (side == Color::White) ? -1 : 1;
                    // Pawn captures only (diagonal)
                    for (int dc : {-1, 1}) {
                        int nr = r + dir;
                        int nc = c + dc;
                        if (nr < 0 || nr > 7 || nc < 0 || nc > 7) continue;
                        Piece target = board.at(nr, nc);
                        if (target.color == enemy) {
                            // Check for promotion
                            if (nr == 0 || nr == 7) {
                                captures.push_back(Move({r, c}, {nr, nc}, MoveType::Promotion, PieceKind::Queen));
                            } else {
                                captures.push_back(Move({r, c}, {nr, nc}));
                            }
                        }
                    }
                    // En passant
                    if (board.enPassantTarget.has_value()) {
                        auto ep = board.enPassantTarget.value();
                        if (ep.r == r + dir && std::abs(ep.c - c) == 1) {
                            captures.push_back(Move({r, c}, {ep.r, ep.c}, MoveType::EnPassant));
                        }
                    }
                    break;
                }

                case PieceKind::Knight: {
                    constexpr int knightMoves[8][2] = {{-2,-1},{-2,1},{-1,-2},{-1,2},{1,-2},{1,2},{2,-1},{2,1}};
                    for (const auto& [dr, dc] : knightMoves) {
                        int nr = r + dr, nc = c + dc;
                        if (nr < 0 || nr > 7 || nc < 0 || nc > 7) continue;
                        if (board.at(nr, nc).color == enemy) {
                            captures.push_back(Move({r, c}, {nr, nc}));
                        }
                    }
                    break;
                }

                case PieceKind::Bishop:
                    generateSlidingCaptures(board, r, c, captures, enemy, true, false);
                    break;

                case PieceKind::Rook:
                    generateSlidingCaptures(board, r, c, captures, enemy, false, true);
                    break;

                case PieceKind::Queen:
                    generateSlidingCaptures(board, r, c, captures, enemy, true, true);
                    break;

                case PieceKind::King: {
                    for (int dr = -1; dr <= 1; dr++) {
                        for (int dc = -1; dc <= 1; dc++) {
                            if (dr == 0 && dc == 0) continue;
                            int nr = r + dr, nc = c + dc;
                            if (nr < 0 || nr > 7 || nc < 0 || nc > 7) continue;
                            if (board.at(nr, nc).color == enemy) {
                                captures.push_back(Move({r, c}, {nr, nc}));
                            }
                        }
                    }
                    break;
                }

                default: break;
            }
        }
    }

    return captures;
}
