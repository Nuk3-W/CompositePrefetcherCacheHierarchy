#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include "Core/Types.h"


enum class CacheResult {
    Hit,
    Miss,
    Evict
};

struct LevelStats {
    uint64_t reads_{};
    uint64_t readMisses_{};
    uint64_t writes_{};
    uint64_t writeMisses_{};
    
    uint64_t swapRequests_{};
    uint64_t swaps_{};
    
    uint64_t writebacks_{};
    
    uint64_t hits_{};
    uint64_t misses_{};
    uint64_t evictions_{};
};

// singleton class for easy logging
class StatisticsManager {
public:
    static StatisticsManager& getInstance();
    

    void recordAccess(std::size_t level, AccessType accessType, CacheResult result);
    void recordSwapRequest(std::size_t level);
    void recordSwap(std::size_t level);
    void recordWriteback(std::size_t level);
    void recordSuperBlockAccess(std::size_t level, CacheResult result);

    void printDetailedStats() const;

private:
    void ensureLevelExists(std::size_t level);
    StatisticsManager() = default;
    StatisticsManager(const StatisticsManager&) = delete;
    StatisticsManager(StatisticsManager&&) = delete;
    StatisticsManager& operator=(const StatisticsManager&) = delete;
    StatisticsManager& operator=(StatisticsManager&&) = delete;
    ~StatisticsManager() = default;

    std::vector<LevelStats> levelStats_;
    uint64_t superBlockHits_{};
    uint64_t superBlockMisses_{};
    uint64_t totalAccesses_{};
}; 