#ifndef CONTROL_UNIT_H
#define CONTROL_UNIT_H

#include <vector>
#include <cstdint>
#include <memory>
#include <optional>
#include <algorithm>
#include <cmath>

#include "Prefetcher/Prefetcher.h"
#include "SystemCacheData.h"


class ControlUnit {
public:
    ControlUnit(const ControlUnitParams& params, const unsigned long blockSize);
    
    void updateTrackerOnAccess(Address currentAddr);
    void updateThresholdOnMiss(int markovTotal, int markovHit, int sequentialTotal, int sequentialHit);

    Address prefetch(Address addr);
    Address readPrefetchedAddress() const;

private:
    void updateSuperBlockTracker(Address addr);
    std::optional<int> findSuperBlockEntry(Address superBlockAddr) const;
    int getVictimLRU() const;
    void updateLRU(int accessedIndex); 
    Address extractSuperBlock(Address addr) const;
private:
    struct SuperBlockEntry {
        Address superBlockAddr_{};
        uint32_t hitCounter_{};
        uint32_t lruCounter_{};
    };

    Prefetcher prefetcher_{};
    std::vector<SuperBlockEntry> superBlockTracker_;
    
    Address superBlockMask_{};
    double threshold_{50};
    PrefetchType currentPrefetcher_{ PrefetchType::Sequential };
};

#endif // CONTROL_UNIT_H
