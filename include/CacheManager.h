#ifndef CACHE_MANAGER_H
#define CACHE_MANAGER_H

#include <memory>
#include <array>
#include <functional>
#include <optional>

#include "Caches/LevelCache.h"
#include "Caches/CacheData.h"
#include "Caches/VictimCache.h"
#include "Prefetcher/ControlUnit.h"
#include "SystemCacheData.h"

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

    std::vector<LevelCache> caches_;
    std::optional<ControlUnit> controlUnit_;
};

#endif