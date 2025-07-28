#include "Core/CacheManager.h"

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
    // Step 1: Update access tracking
    if (controlUnit_) controlUnit_->updateTrackerOnAccess(addr);
    
    // Step 2: Try L1 cache access
    AccessResult cacheResult = accessFunc(caches_[l1CacheIndex_], addr);
    if (isType<Hit>(cacheResult)) return;

    // Step 3: Check for prefetch hits
    if (controlUnit_) {
        AccessResult prefetchResult = checkPrefetch(addr);
        if (isType<Hit>(prefetchResult)) return;
    }

    // Step 4: Handle cache miss
    handleCacheMiss(addr, cacheResult);
}

void CacheManager::handleCacheMiss(Address addr, AccessResult cacheResult) {
    // Update miss tracking statistics
    if (controlUnit_) controlUnit_->updateThresholdOnMiss(addr);

    // Pull data from lower cache levels
    pullFromLowerLevels(addr, cacheResult);

    // Always prefetch on miss to improve future performance
    if (controlUnit_) prefetch(addr);
}

void CacheManager::pullFromLowerLevels(Address addr, AccessResult cacheResult) {
    // Traverse the cache hierarchy from L2 onwards
    for (std::size_t level = l2CacheStart_; level < caches_.size(); ++level) {
        // Handle any evictions from previous level
        if (isType<Evict>(cacheResult)) {
            processWriteBack(cacheResult, level);
        }
        
        // Try to access this cache level
        cacheResult = caches_[level].read(addr);
        if (isType<Hit>(cacheResult)) {
            return; // Found the data at this level
        }
    }
}

void CacheManager::processWriteBack(AccessResult evictedBlock, std::size_t level) {
    if (level >= caches_.size()) return;

    // Write the evicted block to this cache level
    AccessResult nextEviction = writeEvictedBlockToLevel(evictedBlock, level);
    if (isType<Hit>(nextEviction)) return;

    // If this level also evicts a block, recursively handle it
    if (isType<Evict>(nextEviction)) {
        processWriteBack(nextEviction, level + 1);
    }

    // Maintain hierarchy integrity by traversing remaining levels
    traverseCacheHierarchy(getAddress<Evict>(evictedBlock), level + 1);
}

AccessResult CacheManager::writeEvictedBlockToLevel(AccessResult evictedBlock, std::size_t level) {
    return caches_[level].write(getAddress<Evict>(evictedBlock));
}

void CacheManager::traverseCacheHierarchy(Address addr, std::size_t startLevel) {
    for (std::size_t i = startLevel; i < caches_.size(); ++i) {
        AccessResult result = caches_[i].read(addr);
        if (isType<Hit>(result)) return;
    }
}

void CacheManager::prefetch(Address addr) {
    AccessResult prefetchResult = controlUnit_->prefetch(addr);
    pullFromLowerLevels(getAddress<Prefetch>(prefetchResult), prefetchResult);
}

AccessResult CacheManager::checkPrefetch(Address addr) {
    AccessResult prefetchResult = controlUnit_->readPrefetchedAddress(addr);
    // A prefetch request here means we hit in the prefetcher and are continuing the prefetch stream
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


