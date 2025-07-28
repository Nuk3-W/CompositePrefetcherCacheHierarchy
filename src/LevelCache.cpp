#include "Core/LevelCache.h"

// Using directives for cleaner code
using Config::CacheParams;

LevelCache::LevelCache(const CacheParams& params, const CacheParams& vParams) :
	BaseCache(params),
	victimCache_(vParams.size_ > 0 ? std::make_optional<VictimCache>(vParams) : std::nullopt)
{}

AccessResult LevelCache::handleHit(Address setIndex, int way, AccessType type) {
    if (type == AccessType::Write) {
        setDirty(cache_[setIndex + way]);
        updateWriteStats(true);
    } else {
        updateReadStats(true);
    }
    
    updateLRU(setIndex, way);
    return Hit{};
}

AccessResult LevelCache::handleMiss(Address setIndex, Address addr, AccessType type) {
    const int victimIndex = getVictimLRU(setIndex);
    CacheBlock& victimBlock = cache_[setIndex + victimIndex];

    AccessResult evictedAddr{};
    
    // Check if we have a victim cache and the victim block is valid
    if (victimCache_ && isValidBlock(victimBlock)) {
        ++stats_.swapRequests_; // Increment for every swap request
        evictedAddr = handleVictim(victimBlock, addr);
    } else {
        // No victim cache or invalid block - use normal eviction
        evictedAddr = handleCacheEviction(victimBlock, addr);
    }

    // Insert the new block into the victim slot
    victimBlock.addr_ = addr;
    setValid(victimBlock);
    updateLRU(setIndex, victimIndex);

    type == AccessType::Write ? updateWriteStats(false) : updateReadStats(false);
    // reasoning behind weird control flow is the dirty bit needs to be altered before exit on vc swap
    if (type == AccessType::Write) setDirty(victimBlock);
    if (std::holds_alternative<Hit>(evictedAddr)) {
        stats_.swapHits_++;
        return evictedAddr;
    }

    if (std::holds_alternative<Evict>(evictedAddr)) stats_.writeBacks_++;
    if (type == AccessType::Read) clearDirty(victimBlock);

    return evictedAddr;
}

AccessResult LevelCache::access(Address addr, AccessType type) {
    Address setIndex;
    const auto hitWay = findHitWay(addr, setIndex);
    
    if (hitWay) {
        return handleHit(setIndex, *hitWay, type);
    }

    return handleMiss(setIndex, addr, type);
}

AccessResult LevelCache::read(Address addr) {
	return access(addr, AccessType::Read);
}

AccessResult LevelCache::write(Address addr) {
	return access(addr, AccessType::Write);
}

AccessResult LevelCache::handleVictim(CacheBlock& evict, Address addr) {
    return victimCache_->swapReq(evict, addr);
}

void LevelCache::printStats() const {
    std::cout << "===== Simulation results =====\n";
    std::cout << "  a. number of reads:          " << std::setw(20) << stats_.reads_ << "\n";
    std::cout << "  b. number of read misses:    " << std::setw(20) << stats_.readMiss_ << "\n";
    std::cout << "  c. number of writes:         " << std::setw(20) << stats_.writes_ << "\n";
    std::cout << "  d. number of write misses:   " << std::setw(20) << stats_.writeMiss_ << "\n";
    std::cout << "  e. number of swap requests:  " << std::setw(20) << stats_.swapRequests_ << "\n";
    double swapRate = stats_.reads_ + stats_.writes_ ? static_cast<double>(stats_.swapRequests_) / (stats_.reads_ + stats_.writes_) : 0.0;
    std::cout << "  f. swap request rate:        " << std::setw(20) << std::fixed << std::setprecision(4) << swapRate << "\n";
    std::cout << "  g. number of swaps:          " << std::setw(20) << stats_.swapHits_ << "\n";
    std::cout << "  i. number writebacks:        " << std::setw(20) << stats_.writeBacks_ << "\n";
    // L2 stats would be printed by the L2 cache's printStats
}
