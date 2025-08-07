#include "Core/LevelCache.h"

LevelCache::LevelCache(const Config::CacheParams& mainCacheParams, const Config::CacheParams& victimCacheParams, std::size_t levelIndex)
    : mainCache_(mainCacheParams), levelIndex_(levelIndex) {
    if (victimCacheParams.size_ > 0) {
        victimCache_ = CacheContainerManager(victimCacheParams);
    }
}

AccessResult LevelCache::read(Address addr) {
    AccessResult mainResult = mainCache_.read(addr);
    if (Utils::isType<Hit>(mainResult)) {
        StatisticsManager::getInstance().recordAccess(levelIndex_, AccessType::Read, CacheResult::Hit);
        return mainResult;
    }
    
    StatisticsManager::getInstance().recordAccess(levelIndex_, AccessType::Read, CacheResult::Miss);
    
    if (victimCache_) {
        return handleVictimCacheAccess(addr, mainResult);
    }
    
    return mainResult;
}

AccessResult LevelCache::write(Address addr) {
    AccessResult mainResult = mainCache_.write(addr);
    if (Utils::isType<Hit>(mainResult)) {
        StatisticsManager::getInstance().recordAccess(levelIndex_, AccessType::Write, CacheResult::Hit);
        Utils::getBlock(mainResult).setDirty();
        return mainResult;
    }
    
    StatisticsManager::getInstance().recordAccess(levelIndex_, AccessType::Write, CacheResult::Miss);
    
    if (victimCache_) {
        AccessResult victimResult = handleVictimCacheAccess(addr, mainResult);
        Utils::getBlock(victimResult).setDirty();
        return victimResult;
    }
    
    return mainResult;
}

AccessResult LevelCache::handleVictimCacheAccess(Address addr, AccessResult& mainResult) {
    // we dont need swap requests if set is full because we never transfer data to victim cache
    if (!Utils::getBlock(mainResult).isValid()) {
        return mainResult;
    }

    StatisticsManager::getInstance().recordSwapRequest(levelIndex_);

    AccessResult victimResult = victimCache_->read(addr);
    
    std::swap(Utils::getBlock(mainResult), Utils::getBlock(victimResult));
    
    auto& blockInMain = Utils::getBlock(mainResult);
    if (Utils::isType<Hit>(victimResult)) {
        StatisticsManager::getInstance().recordSwap(levelIndex_);
        return Hit{blockInMain};
    } else if (Utils::isType<Evict>(victimResult)) {
        return Evict{blockInMain};
    } else {
        return Miss{blockInMain};
    }
}

