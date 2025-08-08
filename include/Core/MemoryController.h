#pragma once

#include "Core/LevelCache.h"
#include "Core/EvictionHandler.h"
#include "Config/SystemParams.h"
#include "Core/Types.h"
#include "Utils/VariantUtils.h"

#include <vector>
#include <memory>

class MemoryController {
public:
    MemoryController(const Config::SystemParams& params);
    ~MemoryController() = default;

    void read(Address addr);
    void write(Address addr);
    
private:
    void handleCacheMiss(Address addr, AccessResult cacheResult, AccessType accessType);
    void probeLowerLevelsAndInstall(Address addr);

private:
    std::vector<LevelCache> caches_;
    EvictionHandler evictionHandler_;
    
    static constexpr std::size_t rootLevelIndex_ = 0;
    static constexpr std::size_t firstLowerLevelIndex_ = 1;
}; 