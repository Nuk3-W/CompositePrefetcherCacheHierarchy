#include "CacheManager.h"

CacheManager::CacheManager(const SystemCacheParams& params){
    // Initialize normal caches
    for (const CacheParams& cacheParam : params.caches_) {
        caches_.emplace_back(LevelCache(cacheParam));
    }

    // Initialize victim caches
    for (const CacheParams& cacheParam : params.vCaches_) {
        vCaches_.emplace_back(VictimCache(cacheParam));
    }
}

CacheManager::~CacheManager() = default;

void CacheManager::read(Address addr) {
    Address writeBack = caches_[0].read(addr);
    if (isCacheHit(writeBack)) return;

    for (std::size_t level = 1; level < caches_.size(); ++level) {
        if (writeBack != addr)
		    handleLevelWriteBack(writeBack, level); 

        writeBack = caches_[level].read(addr); 

        if (isCacheHit(writeBack)) return;
    } 
}

void CacheManager::write(Address addr) {
	//because we dont want to write all the way down to memory we only need one write call
    Address writeBack = caches_[0].write(addr); 
    if (isCacheHit(writeBack)) return;

    for (std::size_t level = 1; level < caches_.size(); ++level) {
        // we dont check for hit because we would already return by now
        if (writeBack != addr)
            handleLevelWriteBack(writeBack, level); 
		// read every other cache to pull up the correct block if needed
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

void CacheManager::handleVictimWriteBack(Address writeBack, std::size_t level) {
}

bool CacheManager::isCacheHit(Address writeBack) const {
    return writeBack == g_invalidAddress;
}


