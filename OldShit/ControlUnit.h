#pragma once

#include <vector>
#include <iostream>
#include <cstdint>
#include <memory>
#include <optional>
#include <algorithm>
#include <cmath>
#include <array>
#include <deque>

#include "Prefetcher.h"
#include "Types.h"
#include "Config/PrefetcherParams.h"

// Using directives for cleaner code
using Config::ControlUnitParams;

class ControlUnit {
public:
    ControlUnit(const ControlUnitParams& params, const uint32_t blockSize);
    
    void updateTrackerOnAccess(Address currentAddr);
    void updateThresholdOnMiss(Address missAddr);

    AccessResult prefetch(Address addr);
    AccessResult readPrefetchedAddress(Address addr) const;
    Address getBlockMask() const { return prefetcher_.getBlockMask(); }

    void updateOnHit();
    void updateOnMiss();
    void resetStats();
    void printStats() const;
    void printQueueStats() const;

private:
    void updateSuperBlockTracker(Address addr);
    std::optional<int> findSuperBlockEntry(Address superBlockAddr) const;
    int getVictimLRU() const;
    void updateLRU(int accessedIndex); 
    Address extractSuperBlock(Address addr) const;
    void addToQueue(bool isHit, PrefetchType type);
    double calculateRecentHitRate(PrefetchType type) const;
private:
    struct SuperBlockEntry {
        Address superBlockAddr_{};
        uint32_t hitCounter_{};
        uint32_t lruCounter_{};
    };

    struct PrefetchStats {
        int markovTotal{};
        int markovHits{};
        int sequentialTotal{};
        int sequentialHits{};
    };

    Prefetcher prefetcher_{};
    std::vector<SuperBlockEntry> superBlockTracker_;
    PrefetchStats prefetchStats_{};
    Address superBlockMask_{};
    double threshold_{50};
    PrefetchType currentPrefetcher_{ PrefetchType::Sequential };
    
    // Recent performance tracking queue (25 entries)
    static constexpr size_t QUEUE_SIZE = 25;
    std::deque<std::pair<bool, PrefetchType>> recentPerformance_;
};
