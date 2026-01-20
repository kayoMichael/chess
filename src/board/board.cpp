#include "board/board.h"

#include <iostream>
#include <cassert>
#include <sstream>
#include <cstring>
#include <cctype>
#include <stdexcept>
#include <cctype>
#include <stdexcept>
#include <cmath>
#include <vector>

#include "movement_const.h"

Board::Board() {
    init();
}

Board::Board(const std::string& fen) {
    loadFEN(fen);
}

void Board::init() {
    for (auto& row : board)
        for (auto& cell : row)
            cell = Piece(PieceKind::None, Color::None);

    for (int j = 0; j < 8; j++) {
        setAt(6, j, Piece(PieceKind::Pawn, Color::White));
        setAt(1, j, Piece(PieceKind::Pawn, Color::Black));
    }

    Piece whiteBack[8] = {
        Piece(PieceKind::Rook, Color::White), Piece(PieceKind::Knight, Color::White), Piece(PieceKind::Bishop, Color::White), Piece(PieceKind::Queen, Color::White),
        Piece(PieceKind::King, Color::White), Piece(PieceKind::Bishop, Color::White), Piece(PieceKind::Knight, Color::White), Piece(PieceKind::Rook, Color::White)
    };

    Piece blackBack[8] = {
        Piece(PieceKind::Rook, Color::Black), Piece(PieceKind::Knight, Color::Black), Piece(PieceKind::Bishop, Color::Black), Piece(PieceKind::Queen, Color::Black),
        Piece(PieceKind::King, Color::Black), Piece(PieceKind::Bishop, Color::Black), Piece(PieceKind::Knight, Color::Black), Piece(PieceKind::Rook, Color::Black)
    };

    for (int j = 0; j < 8; j++) {
        setAt(7, j, whiteBack[j]);
        setAt(0, j, blackBack[j]);
    }
}

void Board::loadFEN(const std::string& fen) {
    auto fail = [](const std::string& msg) {
        throw std::invalid_argument("Invalid FEN: " + msg);
    };

    std::istringstream ss(fen);
    std::string piecePlacement, activeColor, castling, enPassant;

    if (!(ss >> piecePlacement >> activeColor >> castling >> enPassant)) {
        fail("must have at least 4 fields");
    }

    // Validate piece placement structure
    int row = 0, col = 0;
    for (char ch : piecePlacement) {
        if (ch == '/') {
            if (col != 8) fail("incomplete row");
            row++;
            col = 0;
        } else if (std::isdigit(ch)) {
            col += ch - '0';
            if (col > 8) fail("too many squares in row");
        } else if (::strchr("pnbrqkPNBRQK", ch)) {
            col++;
            if (col > 8) fail("too many squares in row");
        } else {
            fail("invalid piece character: " + std::string(1, ch));
        }
    }
    if (row != 7 || col != 8) fail("must have 8 complete rows");

    // Validate active color
    if (activeColor != "w" && activeColor != "b") {
        fail("active color must be 'w' or 'b'");
    }

    // Validate castling
    if (castling != "-") {
        for (char ch : castling) {
            if (!::strchr("KQkq", ch)) {
                fail("invalid castling character: " + std::string(1, ch));
            }
        }
    }

    // Validate en passant
    if (enPassant != "-") {
        if (enPassant.length() != 2) fail("en passant must be 2 characters");
        if (enPassant[0] < 'a' || enPassant[0] > 'h') fail("invalid en passant file");
        if (enPassant[1] != '3' && enPassant[1] != '6') fail("en passant rank must be 3 or 6");
    }

    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            setAt(r, c, Piece(PieceKind::None, Color::None));
        }
    }

    row = 0; col = 0;
    for (char ch : piecePlacement) {
        if (ch == '/') {
            row++;
            col = 0;
        } else if (std::isdigit(ch)) {
            col += ch - '0';
        } else {
            Color color = std::isupper(ch) ? Color::White : Color::Black;
            PieceKind kind = charToKind(static_cast<char>(std::tolower(ch)));
            setAt(row, col, Piece(kind, color));
            col++;
        }
    }

    setSide(activeColor == "w" ? Color::White : Color::Black);

    whiteKingMoved = (castling.find('K') == std::string::npos && castling.find('Q') == std::string::npos);
    whiteRookKingsideMoved = (castling.find('K') == std::string::npos);
    whiteRookQueensideMoved = (castling.find('Q') == std::string::npos);
    blackKingMoved = (castling.find('k') == std::string::npos && castling.find('q') == std::string::npos);
    blackRookKingsideMoved = (castling.find('k') == std::string::npos);
    blackRookQueensideMoved = (castling.find('q') == std::string::npos);

    if (enPassant != "-") {
        int c = enPassant[0] - 'a';
        int r = 8 - (enPassant[1] - '0');
        enPassantTarget = Square(r, c);
    } else {
        enPassantTarget = std::nullopt;
    }
}

std::string Board::toFEN() const {
    std::ostringstream ss;

    for (int r = 0; r < 8; r++) {
        int empty = 0;
        for (int c = 0; c < 8; c++) {
            Piece p = at(r, c);
            if (p.kind == PieceKind::None) {
                empty++;
            } else {
                if (empty > 0) {
                    ss << empty;
                    empty = 0;
                }
                ss << kindToChar(p.kind, p.color);
            }
        }
        if (empty > 0) ss << empty;
        if (r < 7) ss << '/';
    }

    ss << ' ' << (side == Color::White ? 'w' : 'b') << ' ';

    std::string castling;
    if (!whiteRookKingsideMoved && !whiteKingMoved) castling += 'K';
    if (!whiteRookQueensideMoved && !whiteKingMoved) castling += 'Q';
    if (!blackRookKingsideMoved && !blackKingMoved) castling += 'k';
    if (!blackRookQueensideMoved && !blackKingMoved) castling += 'q';
    ss << (castling.empty() ? "-" : castling) << ' ';

    if (enPassantTarget.has_value()) {
        Square ep = enPassantTarget.value();
        ss << static_cast<char>('a' + ep.c) << (8 - ep.r);
    } else {
        ss << '-';
    }

    ss << " 0 1";
    return ss.str();
}

std::optional<Move> Board::parseUCI(const std::string& uci) const {
    auto fail = [](const std::string& msg) -> std::optional<Move> {
        throw std::invalid_argument("Invalid UCI: " + msg);
    };

    if (uci.length() < 4 || uci.length() > 5) {
        return fail("must be 4-5 characters");
    }

    if (uci[0] < 'a' || uci[0] > 'h') return fail("invalid from file");
    if (uci[1] < '1' || uci[1] > '8') return fail("invalid from rank");
    if (uci[2] < 'a' || uci[2] > 'h') return fail("invalid to file");
    if (uci[3] < '1' || uci[3] > '8') return fail("invalid to rank");

    int fromCol = uci[0] - 'a';
    int fromRow = 8 - (uci[1] - '0');
    int toCol = uci[2] - 'a';
    int toRow = 8 - (uci[3] - '0');

    Square from(fromRow, fromCol);
    Square to(toRow, toCol);

    Piece piece = at(fromRow, fromCol);
    if (piece.kind == PieceKind::None) {
        return fail("no piece at source square");
    }

    if (uci.length() == 5) {
        if (!::strchr("qrbn", uci[4])) {
            return fail("invalid promotion piece");
        }
        PieceKind promo = charToKind(uci[4]);
        return Move(from, to, MoveType::Promotion, promo);
    }

    if (piece.kind == PieceKind::King && std::abs(toCol - fromCol) == 2) {
        return Move(from, to, MoveType::Castle);
    }

    if (piece.kind == PieceKind::Pawn && toCol != fromCol &&
        at(toRow, toCol).kind == PieceKind::None) {
        return Move(from, to, MoveType::EnPassant);
        }

    return Move(from, to);
}

std::string Board::toUCI(const Move& move) {
    std::ostringstream ss;
    ss << static_cast<char>('a' + move.current.c)
       << (8 - move.current.r)
       << static_cast<char>('a' + move.destination.c)
       << (8 - move.destination.r);

    if (move.type == MoveType::Promotion) {
        char promo;
        switch (move.promotion) {
            case PieceKind::Queen:  promo = 'q'; break;
            case PieceKind::Rook:   promo = 'r'; break;
            case PieceKind::Bishop: promo = 'b'; break;
            case PieceKind::Knight: promo = 'n'; break;
            default: promo = '?';
        }
        ss << promo;
    }

    return ss.str();
}

void Board::setAt(int r, int c, Piece p) {
    board[r][c] = p;
}

void Board::setSide(Color c) {
    side = c;
}

PieceKind Board::charToKind(char c) {
    switch (c) {
        case 'p': return PieceKind::Pawn;
        case 'n': return PieceKind::Knight;
        case 'b': return PieceKind::Bishop;
        case 'r': return PieceKind::Rook;
        case 'q': return PieceKind::Queen;
        case 'k': return PieceKind::King;
        default:  return PieceKind::None;
    }
}

char Board::kindToChar(PieceKind kind, Color color) {
    char c;
    switch (kind) {
        case PieceKind::Pawn:   c = 'p'; break;
        case PieceKind::Knight: c = 'n'; break;
        case PieceKind::Bishop: c = 'b'; break;
        case PieceKind::Rook:   c = 'r'; break;
        case PieceKind::Queen:  c = 'q'; break;
        case PieceKind::King:   c = 'k'; break;
        default: return '?';
    }
    return color == Color::White ? static_cast<char>(std::toupper(c)) : c;
}

Piece Board::at(const int r, const int c) const {
    assert(r >= 0 && r < 8 && c >= 0 && c < 8);
    return board[r][c];
}

Color Board::getColor() const {
    return side;
}

bool Board::validate(const Move& move) {
    // Check if the square is already occupied by an ally
    Piece dest = at(move.destination.r, move.destination.c);
    if (dest.color == side)
        return false;

    Piece src = at(move.current.r, move.current.c);
    // Cannot move empty square
    if (src.kind == PieceKind::None)
        return false;

    // Cannot move opponent’s piece
    if (src.color != side)
        return false;

    // Hypothetically make the move and check
    Board hypothetical = *this;
    hypothetical.makeMove(move, true);
    Square kingPosition = {};
    bool found = false;
    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            Piece piece = hypothetical.at(r, c);
            if (piece.color == hypothetical.side && piece.kind == PieceKind::King) {
                kingPosition.r = r;
                kingPosition.c = c;
                found = true;
                break;
            }
        }
        if (found) break;
    }

    const Color currSide = (hypothetical.side == Color::White) ? Color::Black : Color::White;

    // Pawn Attack
    if (hypothetical.pawnAttacked(kingPosition, currSide)) return false;

    // Knight Attack
    if (hypothetical.knightAttacked(kingPosition, currSide)) return false;

    // King Attack
    if (hypothetical.kingAttacked(kingPosition, currSide)) return false;

    for (auto [r, c]: MovementConst::CHEBYSHEV_DIRECTIONS) {
        if (hypothetical.directionalAttacked(kingPosition, r, c, currSide)) return false;
    }

    return true;
}

bool Board::isChecked(const Color kingColor) const {
    Square kingPosition = {};
    bool found = false;
    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            Piece piece = at(r, c);
            if (piece.color == kingColor && piece.kind == PieceKind::King) {
                kingPosition.r = r;
                kingPosition.c = c;
                found = true;
                break;
            }
        }
        if (found) break;
    }

    Color attacker = (kingColor == Color::White) ? Color::Black : Color::White;
    if (knightAttacked(kingPosition, attacker) || pawnAttacked(kingPosition, attacker)) return true;
    for (auto [r, c]: MovementConst::CHEBYSHEV_DIRECTIONS) {
        if (directionalAttacked(kingPosition, r, c, attacker)) return true;
    }
    return false;
}

bool Board::squareAttacked(const Square &square, const Color attackerColor) const {
    // Pawn Attack
    if (pawnAttacked(square, attackerColor)) return true;

    // Knight Attack
    if (knightAttacked(square, attackerColor)) return true;

    // King Attack
    if (kingAttacked(square, attackerColor)) return true;

    for (auto [r, c]: MovementConst::CHEBYSHEV_DIRECTIONS) {
        if (directionalAttacked(square, r, c, attackerColor)) return true;
    }

    return false;
}

bool Board::pawnAttacked(const Square piece, const Color attackerColor) const {
    int dir = (attackerColor == Color::White) ? 1 : -1;

    for (int dc : {-1, 1}) {
        const int nr = piece.r + dir;
        const int nc = piece.c + dc;
        if (nr < 0 || nr >= 8 || nc < 0 || nc >= 8) continue;

        Piece p = at(nr, nc);
        if (p.color == attackerColor && p.kind == PieceKind::Pawn)
            return true;
    }
    return false;
}

bool Board::directionalAttacked(const Square piece, const int dr, const int dc, const Color attackerColor) const{
    int r = piece.r + dr;
    int c = piece.c + dc;

    while (r >= 0 && r < 8 && c >= 0 && c < 8) {
        Piece p = at(r, c);

        if (p.color == Color::None) {
            r += dr;
            c += dc;
            continue;
        }
        if (p.color == attackerColor) {
            if ((p.kind == PieceKind::Rook || p.kind == PieceKind::Queen) &&
                (dr == 0 || dc == 0)) {
                return true;
                }

            if ((p.kind == PieceKind::Bishop || p.kind == PieceKind::Queen) &&
                (dr != 0 && dc != 0)) {
                return true;
                }
            return false; // Enemy piece blocking the potential Attacker
        }
        return false; // Our own piece blocks the potential attacker
    }

    return false;
}

bool Board::knightAttacked(const Square piece, const Color attackerColor) const {
    for (auto [dr, dc] : MovementConst::KNIGHT_LATTICE_DISPLACEMENTS) {
        int nr = dr + piece.r;
        int nc = dc + piece.c;
        if (0 <= nr && nr < 8 && 0 <= nc && nc < 8) {
            Piece potentialKnight = at(nr, nc);
            if (attackerColor == potentialKnight.color && potentialKnight.kind == PieceKind::Knight) {
                return true;
            }
        }
    }
    return false;
}

bool Board::kingAttacked(const Square piece, const Color attackerColor) const {
    for (auto [r, c]: MovementConst::CHEBYSHEV_DIRECTIONS) {
        int nr = piece.r + r;
        int nc = piece.c + c;

        if (0 <= nr && nr < 8 && 0 <= nc && nc < 8) {
            Piece p = at(nr, nc);
            if (p.kind == PieceKind::King && p.color == attackerColor) {
                return true;
            }
        }
    }
    return false;
}

std::vector<Piece> Board::pieces() const {
    std::vector<Piece> pieces;

    for (auto& row: board) {
        for (auto& piece: row) {
            if (piece.kind != PieceKind::None) {
                pieces.push_back(piece);
            }
        }
    }
    return pieces;
}

MoveUndo Board::makeMove(const Move& move, const bool hypothetical) {
    MoveUndo undo;
    Piece current_piece = at(move.current.r, move.current.c);

    if (!hypothetical) {
        undo.move = move;
        undo.captured = at(move.destination.r, move.destination.c);
        undo.movedPiece = current_piece;
        undo.whiteKingMoved = whiteKingMoved;
        undo.whiteRookKingsideMoved = whiteRookKingsideMoved;
        undo.whiteRookQueensideMoved = whiteRookQueensideMoved;
        undo.blackKingMoved = blackKingMoved;
        undo.blackRookKingsideMoved = blackRookKingsideMoved;
        undo.blackRookQueensideMoved = blackRookQueensideMoved;
        undo.enPassantTarget = enPassantTarget;
    }

    enPassantTarget = std::nullopt;

    switch (move.type) {
        case MoveType::Normal:
            movePiece(move.current, move.destination);
            if (current_piece.kind == PieceKind::Pawn) {
                int distance = move.destination.r - move.current.r;
                if (distance == 2 || distance == -2) {
                    // En passant target is the square the pawn skipped over
                    enPassantTarget = Square(move.current.r + distance / 2, move.current.c);
                }
            }
            break;

        case MoveType::Promotion:
            movePiece(move.current, move.destination);
            board[move.destination.r][move.destination.c] = Piece(move.promotion, current_piece.color);
            break;

        case MoveType::Castle: {
            // Move king
            movePiece(move.current, move.destination);

            // Move rook
            int row = move.current.r;
            bool kingside = move.destination.c > move.current.c;
            Square rookFrom(row, kingside ? 7 : 0);
            Square rookTo(row, kingside ? 5 : 3);
            movePiece(rookFrom, rookTo);
            break;
        }

        case MoveType::EnPassant: {
            movePiece(move.current, move.destination);
            board[move.current.r][move.destination.c] = Piece(PieceKind::None, Color::None);
            break;
        }

        default: break;
    }

    updateCastlingRights(current_piece, move);

    if (!hypothetical) {
        side = (side == Color::White) ? Color::Black : Color::White;
    }
    return undo;
}

void Board::undoMove(const MoveUndo &undo) {
    switch (undo.move.type) {
        case MoveType::EnPassant:
            // Restore moving pawn
            board[undo.move.current.r][undo.move.current.c] = undo.movedPiece;
            board[undo.move.destination.r][undo.move.destination.c] = Piece(PieceKind::None, Color::None);
            // Restore captured pawn (was beside the moving pawn)
            board[undo.move.current.r][undo.move.destination.c] = undo.captured;
            break;

        case MoveType::Castle: {
            // Restore king
            board[undo.move.current.r][undo.move.current.c] = undo.movedPiece;
            board[undo.move.destination.r][undo.move.destination.c] = Piece(PieceKind::None, Color::None);

            // Restore rook
            int row = undo.move.current.r;
            bool kingside = undo.move.destination.c > undo.move.current.c;
            Square rookFrom(row, kingside ? 5 : 3);  // Where rook ended up
            Square rookTo(row, kingside ? 7 : 0);    // Where rook started
            board[rookTo.r][rookTo.c] = board[rookFrom.r][rookFrom.c];
            board[rookFrom.r][rookFrom.c] = Piece(PieceKind::None, Color::None);
            break;
        }
        default:
            board[undo.move.current.r][undo.move.current.c] = board[undo.move.destination.r][undo.move.destination.c];
            board[undo.move.destination.r][undo.move.destination.c] = undo.captured;
            break;
    }


    whiteKingMoved = undo.whiteKingMoved;
    whiteRookKingsideMoved = undo.whiteRookKingsideMoved;
    whiteRookQueensideMoved = undo.whiteRookQueensideMoved;
    blackKingMoved = undo.blackKingMoved;
    blackRookKingsideMoved = undo.blackRookKingsideMoved;
    blackRookQueensideMoved = undo.blackRookQueensideMoved;
    side = (side == Color::White) ? Color::Black : Color::White;
}

void Board::movePiece(const Square& from, const Square& to) {
    board[to.r][to.c] = board[from.r][from.c];
    board[from.r][from.c] = Piece(PieceKind::None, Color::None);
}

void Board::updateCastlingRights(const Piece& piece, const Move& move) {
    if (piece.kind == PieceKind::King) {
        if (piece.color == Color::White) whiteKingMoved = true;
        else blackKingMoved = true;
    }
    else if (piece.kind == PieceKind::Rook) {
        if (piece.color == Color::White) {
            if (move.current.r == 7 && move.current.c == 7) whiteRookKingsideMoved = true;
            if (move.current.r == 7 && move.current.c == 0) whiteRookQueensideMoved = true;
        } else {
            if (move.current.r == 0 && move.current.c == 7) blackRookKingsideMoved = true;
            if (move.current.r == 0 && move.current.c == 0) blackRookQueensideMoved = true;
        }
    }
}

void Board::print() const {
    std::cout << "  ┌───┬───┬───┬───┬───┬───┬───┬───┐\n";
    for (int row = 0; row < 8; row++) {
        std::cout << 8 - row << " │";
        for (int col = 0; col < 8; col++) {
            Piece p = board[row][col];
            std::cout << " ";
            if (p.kind == PieceKind::None) std::cout << " ";
            else if (p.color == Color::White) {
                switch (p.kind) {
                    case PieceKind::Pawn:   std::cout << "♙"; break;
                    case PieceKind::Knight: std::cout << "♘"; break;
                    case PieceKind::Bishop: std::cout << "♗"; break;
                    case PieceKind::Rook:   std::cout << "♖"; break;
                    case PieceKind::Queen:  std::cout << "♕"; break;
                    case PieceKind::King:   std::cout << "♔"; break;
                    default: break;
                }
            } else {
                switch (p.kind) {
                    case PieceKind::Pawn:   std::cout << "♟"; break;
                    case PieceKind::Knight: std::cout << "♞"; break;
                    case PieceKind::Bishop: std::cout << "♝"; break;
                    case PieceKind::Rook:   std::cout << "♜"; break;
                    case PieceKind::Queen:  std::cout << "♛"; break;
                    case PieceKind::King:   std::cout << "♚"; break;
                    default: break;
                }
            }
            std::cout << " │";
        }
        std::cout << "\n";
        if (row < 7)
            std::cout << "  ├───┼───┼───┼───┼───┼───┼───┼───┤\n";
    }
    std::cout << "  └───┴───┴───┴───┴───┴───┴───┴───┘\n";
    std::cout << "    a   b   c   d   e   f   g   h\n";
}