#include "CacheManager.h"

CacheManager::CacheManager(const SystemCacheParams& params) {
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
    // Try to access the requested address in L1 cache
    Address writeBackAddr = accessFunc(caches_[0], addr);
    if (isCacheHit(writeBackAddr)) return;

    // On L1 miss, check successive cache levels
    for (std::size_t level = 1; level < caches_.size(); ++level) {
        // If L1 evicted a dirty block, write it back to this level
        if (writeBackAddr != addr) {
            handleLevelWriteBack(writeBackAddr, level);
        }
            
        // Read the originally requested address from this level
        writeBackAddr = caches_[level].read(addr);
        if (isCacheHit(writeBackAddr)) return;
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
}


