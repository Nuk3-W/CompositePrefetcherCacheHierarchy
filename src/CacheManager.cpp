#include "CacheManager.h"

CacheManager::CacheManager(const SystemCacheParams& params){
    for (std::size_t i = 0; i < params.caches_.size(); ++i){
		caches_.emplace_back(params.caches_[i], params.vCaches_[i]);
    }
}

void CacheManager::read(Address addr) {
    access(addr, [](LevelCache& cache, Address a) {
        return cache.read(a);
    });
}

void CacheManager::write(Address addr) {
    access(addr, [](LevelCache& cache, Address a) {
        return cache.write(a);
    });
}

void CacheManager::access(Address addr, std::function<Address(LevelCache&, Address)> accessFunc) {
    Address writeBack = accessFunc(caches_[0], addr);
    if (isCacheHit(writeBack)) return;

    for (std::size_t level = 1; level < caches_.size(); ++level) {
        if (writeBack != addr) {
            handleLevelWriteBack(writeBack, level);
        }
            
        writeBack = caches_[level].read(addr);
        if (isCacheHit(writeBack)) return;
    }
}

void CacheManager::handleLevelWriteBack(Address writeBack, std::size_t level) {
    if (level >= caches_.size()) return;

    Address nextEvicted = caches_[level].write(writeBack);
	if (isCacheHit(nextEvicted)) return;

    if (nextEvicted != writeBack) {
        handleLevelWriteBack(nextEvicted, level + 1);
    }

    // Now iteratively read the original block back through remaining levels
    for (std::size_t i = level + 1; i < caches_.size(); ++i) {
        writeBack = caches_[i].read(writeBack);
		if (isCacheHit(writeBack)) return;
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


