#pragma once

enum class Color { None, White, Black };
enum class PieceKind { None, Pawn, Knight, Bishop, Rook, Queen, King };

constexpr int pieceValue(PieceKind kind) {
    switch (kind) {
        case PieceKind::None: return 0;
        case PieceKind::Pawn: return 100;
        case PieceKind::Knight: return 320;
        case PieceKind::Bishop: return 330;
        case PieceKind::Rook: return 500;
        case PieceKind::Queen: return 900;
        case PieceKind::King: return 20000;
        default: return 0;
    }
}

// Punish moving Knight towards the corner of the board.
// Center Knight positions are also not optimal since it gets attacked very easily in the early game.
constexpr int knightPST_EARLY[8][8] = {
    {-50, -40, -30, -30, -30, -30, -40, -50},
    {-40, -20,   0,   0,   0,   0, -20, -40},
    {-30,   0,   5,   5,   5,   5,   0, -30},
    {-30,   0,   5,  10,  10,   5,   0, -30},
    {-30,   0,   5,  10,  10,   5,   0, -30},
    {-30,   0,  15,   5,   5,  15,   0, -30},  // c3/f3 good, d3/e3 okay but impossible to get to unless moving the knight twice.
    {-40, -20,   0,   5,   5,   0, -20, -40},  // d2/e2 fine
    {-50, -40, -30, -30, -30, -30, -40, -50},
};

constexpr int knightPST_LATE[8][8] = {
    {-50, -40, -30, -30, -30, -30, -40, -50},
    {-40, -20,   5,   5,   5,   5, -20, -40},
    {-30,   5,  15,  15,  15,  15,   5, -30},
    {-30,   5,  20,  15,  15,  20,   5, -30},
    {-30,   5,  20,  25,  25,  20,   5, -30},
    {-30,   5,  20,  20,  20,  20,   5, -30},
    {-40, -20,   0,   5,   5,   0, -20, -40},
    {-50, -40, -30, -30, -30, -30, -40, -50},
};

// Early game: Develop to active diagonals (Bc4/Bc5/Bf4/Bf5), fianchetto okay, punish back rank
constexpr int bishopPST_EARLY[8][8] = {
    {-20, -10, -10, -10, -10, -10, -10, -20},
    {-10,   5,   0,   0,   0,   0,   5, -10},
    {-10,  10,  10,   5,   5,  10,  10, -10},
    {-10,  10,  15,   5,   5,  15,  10, -10},
    {-10,  10,  15,   5,   5,  15,  10, -10},
    {-10,   5,  10,  10,  10,  10,   5, -10},
    {-10,  10,   0,   0,   0,   0,  10, -10},
    {-20, -10, -15, -10, -10, -15, -10, -20},
};

// Mid/late game: Central control more valuable, long diagonals strong
constexpr int bishopPST_LATE[8][8] = {
    {-20, -10, -10, -10, -10, -10, -10, -20},
    {-10,   5,   5,   5,   5,   5,   5, -10},
    {-10,   5,  15,  15,  15,  15,   5, -10},
    {-10,   5,  15,  20,  20,  15,   5, -10},
    {-10,   5,  15,  20,  20,  15,   5, -10},
    {-10,   5,  15,  15,  15,  15,   5, -10},
    {-10,  10,   5,   5,   5,   5,  10, -10},
    {-20, -10, -10, -10, -10, -10, -10, -20},
};
// Early game: Punish King Heavily for moving except castling.
// Once castled, adjustment of king in the backrank is not punished.
constexpr int kingPST_EARLY[8][8] = {
    {-80, -80, -80, -80, -80, -80, -80, -80},
    {-80, -80, -80, -80, -80, -80, -80, -80},
    {-80, -80, -80, -80, -80, -80, -80, -80},
    {-80, -80, -80, -80, -80, -80, -80, -80},
    {-80, -80, -80, -80, -80, -80, -80, -80},
    {-60, -60, -60, -60, -60, -60, -60, -60},
    {-20, -20, -20, -40, -40, -40, -20, -20},
    { 20,  30,  15, -30, -10, -30,  30,  20},
};

// Late game: King should be active and centralized
constexpr int kingPST_LATE[8][8] = {
    {-50, -30, -20, -20, -20, -20, -30, -50},
    {-30, -10,   0,   5,   5,   0, -10, -30},
    {-20,   0,  15,  20,  20,  15,   0, -20},
    {-20,   5,  20,  25,  25,  20,   5, -20},
    {-20,   5,  20,  25,  25,  20,   5, -20},
    {-20,   0,  15,  20,  20,  15,   0, -20},
    {  5,   5,   0,   5,   5,   0,   5,   5},
    {-50, -30, -20, -20, -20, -20, -30, -50},
};

// Early game: Stay on back rank, connect rooks, control open files
constexpr int rookPST_EARLY[8][8] = {
    {-30, -30, -30, -30, -30, -30, -30, -30},
    {-30, -30, -30, -30, -30, -30, -30, -30},
    {-30, -30, -30, -30, -30, -30, -30, -30},
    {-30, -30, -30, -30, -30, -30, -30, -30},
    {-30, -30, -30, -30, -30, -30, -30, -30},
    {-30, -30, -30, -30, -30, -30, -30, -30},
    { -5,  -5,  -5,  -5,  -5,  -5,  -5,  -5},
    {  0,   0,   5,  10,  10,   5,   0,   0},
};

// Mid/late game: 7th rank is recommended attacking position, central files good, back rank is okay
constexpr int rookPST_LATE[8][8] = {
    {  0,   0,   5,  10,  10,   5,   0,   0},
    { 15,  15,  20,  25,  25,  20,  15,  15},
    {  0,   0,   5,  10,  10,   5,   0,   0},
    {  0,   0,   5,  10,  10,   5,   0,   0},
    {  0,   0,   5,  10,  10,   5,   0,   0},
    {  0,   0,   5,  10,  10,   5,   0,   0},
    {  0,   0,   5,  10,  10,   5,   0,   0},
    {  0,   0,   5,  10,  10,   5,   0,   0},
};

// Early game: Stay back, don't get chased, modest development okay
constexpr int queenPST_EARLY[8][8] = {
    {-50, -50, -50, -50, -50, -50, -50, -50},
    {-50, -50, -50, -50, -50, -50, -50, -50},
    {-30, -30, -30, -30, -30, -30, -30, -30},
    {-20, -20, -20, -20, -20, -20, -20, -20},
    {-10, -10, -10, -10, -10, -10, -10, -10},
    { -5,  -5,   0,   0,   0,   0,  -5,  -5},
    { -5,   0,   5,   5,   5,   5,   0,  -5},
    { -5,   0,   5,   5,   0,   5,   0,  -5},
};

// Mid/late game: Active queen, central and advanced positions strong
constexpr int queenPST_LATE[8][8] = {
    {-20, -10,   0,   5,   5,   0, -10, -20},
    {-10,   0,  10,  15,  15,  10,   0, -10},
    { -5,   5,  15,  20,  20,  15,   5,  -5},
    { -5,   5,  15,  20,  20,  15,   5,  -5},
    { -5,   5,  10,  15,  15,  10,   5,  -5},
    { -5,   0,  10,  10,  10,  10,   0,  -5},
    {-10,   0,   5,   5,   5,   5,   0, -10},
    {-20, -10,   0,   0,   0,   0, -10, -20},
};

// Early/Mid-game: Prioritise Central control, development, don't push edge pawns
constexpr int pawnPST[8][8] = {
    {  0,   0,   0,   0,   0,   0,   0,   0},  // rank 8 (promotion, reward via material gain)
    { 50,  50,  50,  50,  50,  50,  50,  50},  // rank 7 (about to promote)
    { 20,  20,  25,  30,  30,  25,  20,  20},
    { 10,  10,  15,  25,  25,  15,  10,  10},
    {  5,   5,  20,  20,  20,  10,   5,   5}, // English (Sicillian Black), E4 (E5), Queen's Gambit (d5, Scandanavian)
    {  0,   0,   5,  10,  10,   5,   0,   0},
    {  0,   0,   0,   -5,  -5,   0,   0,   0},
    {  0,   0,   0,   0,   0,   0,   0,   0},
};

constexpr int MATE = 1000000;
constexpr int INF = 1'000'000'000;
constexpr int PHASE_KNIGHT = 1;
constexpr int PHASE_BISHOP = 1;
constexpr int PHASE_ROOK   = 2;
constexpr int PHASE_QUEEN  = 4;
constexpr int MAX_PHASE = 24;

struct Piece {
    PieceKind kind;
    Color color;

    Piece() = default;
    Piece(PieceKind k, Color c) : kind(k), color(c) {}
};