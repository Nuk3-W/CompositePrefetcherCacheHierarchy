#pragma once

#include <memory>
#include <array>
#include <functional>
#include <optional>

#include "LevelCache.h"
#include "Config/SystemParams.h"
#include "ControlUnit.h"
#include "Utils/VariantUtils.h"

using Config::SystemCacheParams;

/**
 * Manages a multi-level cache hierarchy with prefetching capabilities.
 * Handles cache accesses, writebacks, and prefetch operations across
 * multiple cache levels (L1, L2, etc.) with optional victim caches and 
 * stream buffers.
 */
class CacheManager {
public:
    CacheManager(const SystemCacheParams& systemCacheParams);

    void read(Address addr);
    void write(Address addr);
    void printStats() const;

private:
    static constexpr std::size_t l1CacheIndex_ = 0;
    static constexpr std::size_t l2CacheStart_ = 1;

    // Main access flow
    void access(Address addr, std::function<AccessResult(LevelCache&, Address)> accessFunc);
    
    // Cache hierarchy management
    void pullFromLowerLevels(Address addr, AccessResult cacheResult);
    void processWriteBack(AccessResult evictedBlock, std::size_t level);

    // Prefetch management
    AccessResult checkPrefetch(Address addr);
    void prefetch(Address addr);

    // Helper methods
    void handleCacheMiss(Address addr, AccessResult cacheResult);
    AccessResult writeEvictedBlockToLevel(AccessResult evictedBlock, std::size_t level);
    void traverseCacheHierarchy(Address addr, std::size_t startLevel);

    // Member variables
    std::vector<LevelCache> caches_;
    std::optional<ControlUnit> controlUnit_;
    uint32_t blockSize_;
};