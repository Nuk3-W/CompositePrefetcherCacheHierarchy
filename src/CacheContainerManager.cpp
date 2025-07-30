#include "Core/CacheContainerManager.h"

CacheContainerManager::CacheContainerManager(const CacheParams& params) 
    : cache_(params), replacementPolicy_() {
}

std::optional<std::reference_wrapper<CacheBlock>> CacheContainerManager::findAddr(Address addr) {
    return cache_.findBlock(addr);
}

std::reference_wrapper<CacheBlock> CacheContainerManager::evict(Address addr) {
    return replacementPolicy_.evict(cache_, addr);
}

