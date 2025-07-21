#include "Caches/LevelCache.h"

LevelCache::LevelCache(const CacheParams& params, const CacheParams& vParams) :
	BaseCache(params),
	victimCache_(vParams.size_ > 0 ? std::make_optional<VictimCache>(vParams) : std::nullopt)
{}

Address LevelCache::handleHit(Address setIndex, int way, AccessType type) {
    if (type == AccessType::Write) {
        setDirty(cache_[setIndex + way]);
        updateWriteStats(true);
    } else {
        updateReadStats(true);
    }
    
    updateLRU(setIndex, way);
    return g_cacheHitAddress;
}

Address LevelCache::handleMiss(Address setIndex, Address addr, AccessType type) {
    // allocate a victim block
    int victimIndex = getVictimLRU(setIndex);
    CacheBlock& victimBlock = cache_[setIndex + victimIndex];

    Address evictedAddr{};
    // If the victim is valid and we have a victim cache, use the victim cache; if not, do normal eviction
    if (victimCache_ && isValidBlock(victimBlock)) {
        evictedAddr = handleVictim(victimBlock, addr);
    } else {
        evictedAddr = handleCacheEviction(victimBlock, addr);
    }

    // Insert the new block and update stats
    victimBlock.addr_ = addr;
    updateLRU(setIndex, victimIndex);

    if (type == AccessType::Write) {
        setDirty(victimBlock);
        updateWriteStats(false);
    } else {
        clearDirty(victimBlock);
        updateReadStats(false);
    }

    return evictedAddr;
}

Address LevelCache::access(Address addr, AccessType type) {
    Address setIndex;
    // has reference to modify setIndex
    std::optional<int> hitWay = findHitWay(addr, setIndex);
    if (hitWay) {
        return handleHit(setIndex, *hitWay, type);
    }

    return handleMiss(setIndex, addr, type);
}

Address LevelCache::read(Address addr) {
	return access(addr, AccessType::Read);
}

Address LevelCache::write(Address addr) {
	return access(addr, AccessType::Write);
}

Address LevelCache::handleVictim(CacheBlock& evict, Address addr) {
    return 0;
}

void LevelCache::printStats() const {
	std::cout << "Reads: "        << stats_.reads_     << "\n"
              << "Read Misses: "  << stats_.readMiss_  << "\n"
              << "Writes: "       << stats_.writes_    << "\n"
              << "Write Misses: " << stats_.writeMiss_ << "\n"
              << "Hits: "         << stats_.hits_      << "\n"
              << "Misses: "       << stats_.misses_    << "\n";
}
