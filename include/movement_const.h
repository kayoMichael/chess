#pragma once
#include <utility>

namespace MovementConst {
    // Pawn: restricted Minkowski sum of basis vectors under Z₂
    inline constexpr std::pair<int, int> MINKOWSKI_RESTRICTED_SUM[4] = {
        { 1,  0 },
        { 2,  0 },
        { 1,  1 },
        { 1, -1 }
    };

    // Knight: (2, 1) displacement under D₄ dihedral symmetry
    inline constexpr std::pair<int, int> KNIGHT_LATTICE_DISPLACEMENTS[8] = {
        {-2,  1}, {-1,  2}, { 1,  2}, { 2,  1},
        { 2, -1}, { 1, -2}, {-1, -2}, {-2, -1}
    };

    // Queen, King: rays spanning the Chebyshev unit sphere S∞(0,1)
    inline constexpr std::pair<int, int> CHEBYSHEV_DIRECTIONS[8] = {
        { 1,  0}, {-1,  0}, { 0,  1}, { 0, -1},
        { 1,  1}, { 1, -1}, {-1,  1}, {-1, -1}
    };

    // Rook: axis-aligned generators of the Manhattan metric
    inline constexpr std::pair<int, int> MANHATTAN_BASIS_VECTORS[4] = {
        { 1,  0}, {-1,  0}, { 0,  1}, { 0, -1}
    };

    // Bishop: primitive generators of the diagonal lattice {|x| = |y|}
    inline constexpr std::pair<int, int> DIAGONAL_LATTICE_DIRECTIONS[4] = {
        { 1,  1}, { 1, -1}, {-1,  1}, {-1, -1}
    };
}