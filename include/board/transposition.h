#pragma once

#include <random>
#include <vector>

struct TTEntry {
    uint64_t hash = 0;
    int score = 0;
    int depth = -999;
    uint8_t flag = 0;
};

enum TTFlag { EXACT, LOWER_BOUND, UPPER_BOUND };

class TranspositionTable {
    std::vector<TTEntry> table;
    size_t size;

public:
    TranspositionTable(size_t mb = 64) {
        size = (mb * 1024 * 1024) / sizeof(TTEntry);
        table.resize(size);
    }

    TTEntry* probe(uint64_t hash) {
        TTEntry& entry = table[hash % size];
        if (entry.hash == hash) return &entry;
        return nullptr;
    }

    void store(uint64_t hash, int score, int depth, uint8_t flag) {
        TTEntry& entry = table[hash % size];
        if (depth >= entry.depth) {
            entry = {hash, score, depth, flag};
        }
    }
};