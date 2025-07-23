#ifndef CONTROL_UNIT_H
#define CONTROL_UNIT_H

#include <vector>
#include <cstdint>
#include <optional>
#include <algorithm>
#include "Caches/CacheData.h"

struct ControlUnitParams {
    uint32_t superBlockBits_{12};
    uint32_t kTrackerSize_{16};
    double threshold_{50.0};
};

struct SuperBlockEntry {
    Address superBlockAddr_{};
    uint32_t hitCounter_{};
    uint32_t lruCounter_{};
};

// if you ever need to change the prefetchers or want more than to just remove bool and add prefetcher name then change threshold logic
enum class PrefetcherType : bool {
    Markov = true,
    Sequential = false
};

class ControlUnit {
public:
    ControlUnit(const ControlUnitParams& params);
    
    PrefetcherType getCurrentPrefetcher() const { return currentPrefetcher_; }
    void updateTrackerOnAccess(Address currentAddr);
    void updateThresholdOnMiss(int markovTotal, int markovHit, int sequentialTotal, int sequentialHit);

private:
    void updateSuperBlockTracker(Address addr);
    std::optional<int> findSuperBlockEntry(Address superBlockAddr) const;
    int getVictimLRU() const;
    void updateLRU(int accessedIndex); 
    Address extractSuperBlock(Address addr) const;

    
    std::vector<SuperBlockEntry> superBlockTracker_{};
    Address superBlockMask_{};
    double threshold_{};
    PrefetcherType currentPrefetcher_{};
};

#endif // CONTROL_UNIT_H
