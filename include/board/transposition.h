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
    uint64_t hits = 0; // For logging and benchmark
    uint64_t misses = 0;
    uint64_t stores = 0;

    void resetStats() {
        hits = 0;
        misses = 0;
        stores = 0;
    }

    TranspositionTable(size_t mb = 64) {
        size = (mb * 1024 * 1024) / sizeof(TTEntry);
        table.resize(size);
    }

    void clear() {
        for (auto& entry : table) {
            entry.hash = 0;
            entry.score = 0;
            entry.depth = -999;
            entry.flag = 0;
        }
    }

    TTEntry* probe(uint64_t hash) {
        TTEntry& entry = table[hash % size];
        if (entry.hash == hash && entry.depth >= 0) {
            hits++;
            return &entry;
        }
        misses++;
        return nullptr;
    }

    void store(uint64_t hash, int score, int depth, uint8_t flag) {
        TTEntry& entry = table[hash % size];
        if (depth >= entry.depth) {
            entry = {hash, score, depth, flag};
            stores++;
        }
    }
};