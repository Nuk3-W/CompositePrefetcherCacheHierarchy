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

void CacheManager::access(Address addr, std::function<Address(LevelCache&, Address)> accessFunc) {
    if (controlUnit_) controlUnit_->updateTrackerOnAccess(addr);
    
    // First, try to access the requested address in L1 cache
    Address writeBackAddr = accessFunc(caches_[0], addr);
    if (isCacheHit(writeBackAddr)) return;

    if (controlUnit_) {
        // Check if this address was prefetched
        Address prefetchResult = controlUnit_->readPrefetchedAddress();
        // Compare block addresses, not byte addresses
        //std::cout << "prefetchResult: " << std::hex << prefetchResult << std::dec << std::endl;
        Address requestedBlock = addr & controlUnit_->getBlockMask();
        if (prefetchResult == requestedBlock) {
            controlUnit_->updateOnHit();
            Address nextPrefetch = controlUnit_->prefetch(prefetchResult);
            pullFromLowerLevels(nextPrefetch, nextPrefetch);
            return;
        }
    }

    //std::cout << "miss\n";

    if (controlUnit_) controlUnit_->updateThresholdOnMiss(addr);

    pullFromLowerLevels(addr, writeBackAddr);

    // Update threshold and generate new prefetch
    if (controlUnit_) {
        Address prefetchedAddress = controlUnit_->prefetch(addr);
        //std::cout << "Prefetched address: " << std::hex << prefetchedAddress << std::dec << std::endl;
        if (prefetchedAddress != ~0UL) {
            pullFromLowerLevels(prefetchedAddress, prefetchedAddress);
        }
    }
}

void CacheManager::pullFromLowerLevels(Address addr, Address writeBackAddr) {
    
    // Traverse the cache hierarchy from level 1 onwards
    for (std::size_t level = 1; level < caches_.size(); ++level) {
        if (writeBackAddr != addr) {
            handleLevelWriteBack(writeBackAddr, level);
        }
        writeBackAddr = caches_[level].read(addr);
        if (isCacheHit(writeBackAddr)) {
            return;
        }
    }

}

void CacheManager::handleLevelWriteBack(Address writeBackAddr, std::size_t level) {
    if (level >= caches_.size()) return;

    // Write the evicted block to this cache level
    Address nextEvictedAddr = caches_[level].write(writeBackAddr);
    if (isCacheHit(nextEvictedAddr)) return;

    // If this level also evicts a block, recursively handle it
    if (nextEvictedAddr != writeBackAddr) {
        handleLevelWriteBack(nextEvictedAddr, level + 1);
    }

    // Read the original block back through remaining levels to maintain hierarchy
    for (std::size_t i = level + 1; i < caches_.size(); ++i) {
        writeBackAddr = caches_[i].read(writeBackAddr);
        if (isCacheHit(writeBackAddr)) return;
    }
}

bool CacheManager::isCacheHit(Address writeBack) const {
    return writeBack == g_cacheHitAddress;
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


