 #include "CacheManager.h"

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
    
    // First, try to access the requested address in L1 cache
    AccessResult writeBackAddr = accessFunc(caches_[0], addr);
    if (std::holds_alternative<Hit>(writeBackAddr)) return;

    if (controlUnit_) {
        Address prefetchResult = controlUnit_->readPrefetchedAddress();
        Address requestedBlock = addr & controlUnit_->getBlockMask();
        if (prefetchResult == requestedBlock) {
            controlUnit_->updateOnHit();
            AccessResult nextPrefetch = controlUnit_->prefetch(prefetchResult);
            pullFromLowerLevels(std::get<Prefetch>(nextPrefetch).addr, nextPrefetch);
            return;
        }
    }


    if (controlUnit_) controlUnit_->updateThresholdOnMiss(addr);

    pullFromLowerLevels(addr, writeBackAddr);

    // Update threshold and generate new prefetch
    if (controlUnit_) {
        AccessResult prefetchResult = controlUnit_->prefetch(addr);
        pullFromLowerLevels(std::get<Prefetch>(prefetchResult).addr, prefetchResult);
    }
}

void CacheManager::pullFromLowerLevels(Address addr, AccessResult writeBackAddr) {
    // Traverse the cache hierarchy from level 1 onwards
    for (std::size_t level = 1; level < caches_.size(); ++level) {
        if (std::holds_alternative<Evict>(writeBackAddr)) {
            handleLevelWriteBack(writeBackAddr, level);
        }
        writeBackAddr = caches_[level].read(addr);
        if (std::holds_alternative<Hit>(writeBackAddr)) {
            return;
        }
    }
}

void CacheManager::handleLevelWriteBack(AccessResult writeBackAddr, std::size_t level) {
    if (level >= caches_.size()) return;

    // Write the evicted block to this cache level
    AccessResult nextEvictedAddr = caches_[level].write(std::get<Evict>(writeBackAddr).addr);
    if (std::holds_alternative<Hit>(nextEvictedAddr)) return;

    // If this level also evicts a block, recursively handle it
    if (std::holds_alternative<Evict>(nextEvictedAddr)) {
        handleLevelWriteBack(nextEvictedAddr, level + 1);
    }

    // Read the original block back through remaining levels to maintain hierarchy
    for (std::size_t i = level + 1; i < caches_.size(); ++i) {
        writeBackAddr = caches_[i].read(std::get<Evict>(writeBackAddr).addr);
        if (std::holds_alternative<Hit>(writeBackAddr)) return;
    }
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


