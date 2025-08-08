#pragma once

#include "Core/CacheContainerManager.h"
#include "Config/CacheParams.h"
#include "Core/Types.h"
#include "Utils/VariantUtils.h"
#include "Core/StatisticsManager.h"
#include <memory>
#include <optional>

class LevelCache {
public:
    LevelCache(const Config::CacheParams& mainCacheParams, const Config::CacheParams& victimCacheParams = Config::CacheParams{}, std::size_t levelIndex = 0);
    ~LevelCache() = default;

    AccessResult read(Address addr);
    AccessResult write(Address addr);
private:
    AccessResult handleVictimCacheAccess(Address addr, AccessResult& mainResult);
private:
    CacheContainerManager mainCache_;
    std::optional<CacheContainerManager> victimCache_{ std::nullopt };
    std::size_t levelIndex_;
};
