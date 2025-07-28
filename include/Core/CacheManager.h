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

class CacheManager {
public:
    CacheManager(const SystemCacheParams& systemCacheParams);

    void read(Address addr);
	void write(Address addr);

    void printStats() const;
private:
    void access(Address addr, std::function<AccessResult(LevelCache&, Address)> accessFunc);
    
    void pullFromLowerLevels(Address addr, AccessResult writeBackAddr);
    void handleLevelWriteBack(AccessResult writeBack, std::size_t level);

    AccessResult checkPrefetch(Address addr);
    void prefetch(Address addr);

    std::vector<LevelCache> caches_;
    std::optional<ControlUnit> controlUnit_;
    uint32_t blockSize_;
};