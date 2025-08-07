#include "Core/CacheContainerManager.h"


CacheContainerManager::CacheContainerManager(const Config::CacheParams& params) 
    : cache_(params), replacementPolicy_() {
}

AccessResult CacheContainerManager::read(Address addr) {
    auto block = cache_.findBlock(addr);
    if (block) {
        replacementPolicy_.updateLRU(cache_, addr, block->get());
        return Hit{std::ref(block->get())};
    }
    return evict(addr);
}
    
AccessResult CacheContainerManager::write(Address addr) {
    auto block = cache_.findBlock(addr);
    if (block) {
        replacementPolicy_.updateLRU(cache_, addr, block->get());
        return Hit{std::ref(block->get())};
    }
    return evict(addr);
}

AccessResult CacheContainerManager::evict(Address addr) {
    auto& evictedBlock = replacementPolicy_.evict(cache_, addr).get();
    if (evictedBlock.isDirty()) {
        return Evict{std::ref(evictedBlock)};
    }
    return Miss{std::ref(evictedBlock)};
}
