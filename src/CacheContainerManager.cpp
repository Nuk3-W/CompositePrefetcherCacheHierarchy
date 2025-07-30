#include "Core/CacheContainerManager.h"

CacheContainerManager::CacheContainerManager(const CacheParams& params) 
    : cache_(params), replacementPolicy_() {
}

// All meta data is changed here so further abstraction dont need to worry about it
AccessResult CacheContainerManager::read(Address addr) {
    auto block = cache_.findBlock(addr);
    if (block) {
        return Hit{*block};
    }
    auto evictedBlock = evict(addr);
    if (Utils::isType<Evict>(evictedBlock)) {
        Utils::get<Evict>(evictedBlock).clearDirty();
        return Evict{evictedBlock};
    }
    Utils::get<Miss>(evictedBlock).clearDirty();
    return Miss{evictedBlock};
}
    
AccessResult CacheContainerManager::write(Address addr) {
    auto block = cache_.findBlock(addr);
    if (block) {
        block->setDirty();
        return Hit{*block};
    }
    auto evictedBlock = evict(addr);

    if (Utils::isType<Evict>(evictedBlock)) {
        Utils::get<Evict>(evictedBlock).setDirty();
        return Evict{evictedBlock};
    }
    Utils::get<Miss>(evictedBlock).setDirty();
    return Miss{evictedBlock};
}

AccessResult CacheContainerManager::evict(Address addr) {
    auto evictedBlock = replacementPolicy_.evict(cache_, addr);
    if (evictedBlock.isDirty()) {
        return Evict{evictedBlock};
    }
    return Miss{evictedBlock};
}
