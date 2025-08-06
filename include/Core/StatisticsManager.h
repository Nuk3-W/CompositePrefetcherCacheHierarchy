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
    // Access counts
    uint64_t reads_{};
    uint64_t readMisses_{};
    uint64_t writes_{};
    uint64_t writeMisses_{};
    
    // Victim cache statistics
    uint64_t swapRequests_{};
    uint64_t swaps_{};
    
    // Writeback statistics
    uint64_t writebacks_{};
    
    // Legacy statistics (for backward compatibility)
    uint64_t hits_{};
    uint64_t misses_{};
    uint64_t evictions_{};
};

class StatisticsManager {
public:
    static StatisticsManager& getInstance();
    
    // Simple interface for recording cache operations
    void recordAccess(std::size_t level, AccessType accessType, CacheResult result);
    void recordSwapRequest(std::size_t level);
    void recordSwap(std::size_t level);
    void recordWriteback(std::size_t level);

    // Legacy methods (for backward compatibility)
    void recordHit(std::size_t level);
    void recordMiss(std::size_t level);
    void recordEviction(std::size_t level);
    void recordWriteBack(std::size_t level);

    void printStats() const;
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
    uint64_t totalAccesses_{};
}; 