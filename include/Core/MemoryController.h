#pragma once

#include "Core/LevelCache.h"
#include "Core/EvictionHandler.h"
#include "Core/StatisticsManager.h"
#include "Config/SystemParams.h"
#include "Core/Types.h"
#include "Utils/VariantUtils.h"

#include <vector>
#include <memory>
#include <optional>
#include <functional>

class MemoryController {
public:
    MemoryController(const Config::SystemParams& params);
    ~MemoryController() = default;

    void read(Address addr);
    void write(Address addr);
    
private:
    void access(Address addr, std::function<AccessResult(LevelCache&, Address)> accessFunc, AccessType accessType);
    
    void handleCacheMiss(Address addr, AccessResult cacheResult, AccessType accessType);
    void pullFromLowerLevels(Address addr);
    void traverseCacheHierarchy(Address addr, std::size_t startLevel);
    
    void insertBlock(CacheBlock& block, Address addr, AccessType accessType);

private:
    std::vector<LevelCache> caches_;
    EvictionHandler evictionHandler_;
    
    static constexpr std::size_t l1CacheIndex_ = 0;
    static constexpr std::size_t l2CacheStart_ = 1;
}; 