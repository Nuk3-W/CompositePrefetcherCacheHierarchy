#pragma once

#include "Config/CacheParams.h"
#include "Core/CacheLRUReplacement.h"
#include "Core/CacheContainer.h"
#include "Core/CacheBlock.h"
#include "Core/Types.h"

template<typename Block>
class CacheContainerManager {
public:
    CacheContainerManager(const Config::CacheParams& params)
        : cache_(params), replacementPolicy_() {}
    ~CacheContainerManager() = default;

    AccessResult read(Address addr) {
        auto block = cache_.findBlock(addr);
        if (block) {
            replacementPolicy_.updateLRU(cache_, addr, block->get());
            return Hit{std::ref(static_cast<Block&>(block->get()))};
        }
        return evict(addr);
    }

    AccessResult write(Address addr) {
        auto block = cache_.findBlock(addr);
        if (block) {
            replacementPolicy_.updateLRU(cache_, addr, block->get());
            return Hit{std::ref(static_cast<Block&>(block->get()))};
        }
        return evict(addr);
    }
    
private:
    AccessResult evict(Address addr) {
        auto& evictedBlock = replacementPolicy_.evict(cache_, addr).get();
        if (evictedBlock.isDirty()) {
            return Evict{std::ref(static_cast<Block&>(evictedBlock))};
        }
        return Miss{std::ref(static_cast<Block&>(evictedBlock))};
    }

private:
    CacheContainer<Block> cache_;
    CacheLRUReplacement<Block> replacementPolicy_;
};