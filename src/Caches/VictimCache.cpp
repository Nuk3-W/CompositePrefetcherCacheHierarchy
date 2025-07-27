#include "Caches/VictimCache.h"

VictimCache::VictimCache(const CacheParams& params) : BaseCache(params) {}

// Helper to swap only address, dirty, and valid bits
AccessResult VictimCache::swap(CacheBlock& block, int hitWay, Address setIndex) {
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
    return Hit{};
}

AccessResult VictimCache::insertBlock(CacheBlock& block, Address setIndex, Address addr) {
    int victimIndex = getVictimLRU(setIndex);
    CacheBlock& victimBlock = cache_[setIndex + victimIndex];

    // Default to the requested address (no eviction to next level)
    AccessResult evictedAddr = Miss{};
    
    // If we're replacing a valid dirty block, it needs to be written back
    if (isValidBlock(victimBlock) && isDirtyBlock(victimBlock)) {
        evictedAddr = Evict{victimBlock.addr_};
    }
    
    // Copy the block from main cache to victim cache
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

AccessResult VictimCache::swapReq(CacheBlock& block, Address addr) {
    Address setIndex{};
    std::optional<int> hitWay = findHitWay(addr, setIndex);
    
    if (hitWay) {
        return swap(block, *hitWay, setIndex);
    }
    return insertBlock(block, setIndex, addr);
}
