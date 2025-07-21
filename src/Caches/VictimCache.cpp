#include "Caches/VictimCache.h"

VictimCache::VictimCache(const CacheParams& params) : BaseCache(params) {}

// Helper to swap only address, dirty, and valid bits
Address VictimCache::swap(CacheBlock& block, int hitWay, Address setIndex) {
    CacheBlock& vcBlock = cache_[setIndex + hitWay];

    // Swap addresses
    std::swap(block.addr_, vcBlock.addr_);

    // Swap dirty bits only
    bool blockDirty = isDirtyBlock(block);
    bool vcDirty = isDirtyBlock(vcBlock);

    if (vcDirty) setDirty(block); else clearDirty(block);
    if (blockDirty) setDirty(vcBlock); else clearDirty(vcBlock);

    // Update LRU: block X is MRU in main cache, block V is MRU in VC
    updateLRU(setIndex, hitWay);
    ++stats_.swapHits_;
    return g_cacheHitAddress;
}

// Helper: Insert a block into the victim cache, evicting if necessary
Address VictimCache::insertBlock(CacheBlock& block, Address setIndex, Address addr) {
    int victimIndex = getVictimLRU(setIndex);
    CacheBlock& victimBlock = cache_[setIndex + victimIndex];

    Address evictedAddr = addr;
    if (isValidBlock(victimBlock) && isDirtyBlock(victimBlock)) {
        evictedAddr = victimBlock.addr_;
    }
    victimBlock.addr_ = block.addr_;
    
    if (isDirtyBlock(block)) { 
        setDirty(victimBlock);
    } else {
        clearDirty(victimBlock);
    }

    setValid(victimBlock);
    updateLRU(setIndex, victimIndex);
    return evictedAddr;
}

Address VictimCache::swapReq(CacheBlock& block, Address addr) {
    Address setIndex = 0;
    std::optional<int> hitWay = findHitWay(addr, setIndex);
    if (hitWay) {
        return swap(block, *hitWay, setIndex);
    }
    return insertBlock(block, setIndex, addr);
}
