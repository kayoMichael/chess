#pragma once
#include <chrono>
#include <iostream>
#include <string>
#include <unordered_map>

struct ProfileStats {
    long long ns = 0;
    long long calls = 0;
};

class Profiler {
public:
    static inline std::unordered_map<std::string, ProfileStats> stats;

    static void add(const std::string& name, long long ns) {
        auto& s = stats[name];
        s.ns += ns;
        s.calls += 1;
    }

    static void print() {
        std::cout << "\n==== PROFILER RESULTS ====\n";
        for (auto& [name, s] : stats) {
            double ms = s.ns / 1e6;
            double avg_us = (s.calls ? (s.ns / 1e3) / (double)s.calls : 0.0);
            std::cout << name
                      << " : " << ms << " ms"
                      << " | calls=" << s.calls
                      << " | avg=" << avg_us << " us\n";
        }
        std::cout << "==========================\n";
    }

    static void reset() { stats.clear(); }
};

class ScopedTimer {
    using Clock = std::chrono::high_resolution_clock;
    std::string name;
    Clock::time_point start;

public:
    ScopedTimer(std::string n) : name(std::move(n)), start(Clock::now()) {}
    ~ScopedTimer() {
        auto end = Clock::now();
        long long ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
        Profiler::add(name, ns);
    }
};
