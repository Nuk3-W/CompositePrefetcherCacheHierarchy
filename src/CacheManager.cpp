#include "Core/CacheManager.h"

// Using directives for cleaner code
using Config::SystemCacheParams;
using namespace Utils;

CacheManager::CacheManager(const SystemCacheParams& params) : 
    controlUnit_(params.controlUnit_.kTrackerSize_ > 0 ? std::make_optional<ControlUnit>(params.controlUnit_, params.blockSize_) : std::nullopt) {
    
    for (std::size_t i = 0; i < params.caches_.size(); ++i) {
        caches_.emplace_back(params.caches_[i], params.vCaches_[i]);
    }
}

void CacheManager::read(Address addr) {
    access(addr, [](LevelCache& levelCache, Address targetAddr) {
        return levelCache.read(targetAddr);
    });
}

void CacheManager::write(Address addr) {
    access(addr, [](LevelCache& levelCache, Address targetAddr) {
        return levelCache.write(targetAddr);
    });
}

void CacheManager::access(Address addr, std::function<AccessResult(LevelCache&, Address)> accessFunc) {
    if (controlUnit_) controlUnit_->updateTrackerOnAccess(addr);
    
    AccessResult writeBackAddr = accessFunc(caches_[0], addr);
    if (isType<Hit>(writeBackAddr)) return;

    if (controlUnit_) {
        AccessResult prefetchResult = checkPrefetch(addr);
        if (isType<Hit>(prefetchResult)) return;
    }

    if (controlUnit_) controlUnit_->updateThresholdOnMiss(addr);

    pullFromLowerLevels(addr, writeBackAddr);

    // always prefetch on miss
    if (controlUnit_) prefetch(addr);
}

void CacheManager::pullFromLowerLevels(Address addr, AccessResult writeBackAddr) {
    // Traverse the cache hierarchy from level 1 onwards
    for (std::size_t level = 1; level < caches_.size(); ++level) {
        if (isType<Evict>(writeBackAddr)) {
            handleLevelWriteBack(writeBackAddr, level);
        }
        writeBackAddr = caches_[level].read(addr);
        if (isType<Hit>(writeBackAddr)) {
            return;
        }
    }
}

void CacheManager::handleLevelWriteBack(AccessResult writeBackAddr, std::size_t level) {
    if (level >= caches_.size()) return;

    // Write the evicted block to this cache level
    AccessResult nextEvictedAddr = caches_[level].write(getAddress<Evict>(writeBackAddr));
    if (isType<Hit>(nextEvictedAddr)) return;

    // If this level also evicts a block, recursively handle it
    if (isType<Evict>(nextEvictedAddr)) {
        handleLevelWriteBack(nextEvictedAddr, level + 1);
    }

    // Read the original block back through remaining levels to maintain hierarchy
    for (std::size_t i = level + 1; i < caches_.size(); ++i) {
        writeBackAddr = caches_[i].read(getAddress<Evict>(writeBackAddr));
        if (isType<Hit>(writeBackAddr)) return;
    }
}

void CacheManager::prefetch(Address addr) {
    AccessResult prefetchResult = controlUnit_->prefetch(addr);
    pullFromLowerLevels(getAddress<Prefetch>(prefetchResult), prefetchResult);
}

AccessResult CacheManager::checkPrefetch(Address addr) {
    AccessResult prefetchResult = controlUnit_->readPrefetchedAddress(addr);
    // a prefetch request here means we hit in the prefetcher and are continuing the prefetch stream
    if (isType<Prefetch>(prefetchResult)) {
        controlUnit_->updateOnHit();
        prefetch(getAddress<Prefetch>(prefetchResult));
        return Hit{};
    }
    return Miss{};
}

void CacheManager::printStats() const {
    for (std::size_t i = 0; i < caches_.size(); ++i) {
        std::cout << "Cache Level " << i + 1 << " Stats:\n";
        caches_[i].printStats();
        std::cout << "\n";
    }

    if (controlUnit_) {
        std::cout << "Control Unit Stats:\n";
        controlUnit_->printStats();
        std::cout << "\n";
    }
}


