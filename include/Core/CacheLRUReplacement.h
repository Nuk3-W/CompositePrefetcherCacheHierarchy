#pragma once

#include "Config/CacheParams.h"
#include "Core/CacheContainer.h"
#include "Core/CacheBlock.h"
#include "Core/Types.h"

class CacheLRUReplacement {
public:
    CacheLRUReplacement() = default;
    ~CacheLRUReplacement() = default;

    std::reference_wrapper<CacheBlock> evict(CacheContainer& container, Address addr) {
        // Find the set for this address
        auto setBegin = container.setBegin(addr);
        auto setEnd = container.setEnd(addr);
        
        // Find the least recently used block in this set
        auto lruBlock = setBegin;
        uint8_t minLRU = lruBlock->getLRU();
        
        for (auto it = setBegin; it != setEnd; ++it) {
            if (!it->isValid()) {
                return *it;  // Return invalid block immediately
            }
            if (it->getLRU() < minLRU) {
                minLRU = it->getLRU();
                lruBlock = it;
            }
        }
        
        updateLRU(container, addr, *lruBlock);
        return *lruBlock;
    }
    
    void updateLRU(CacheContainer& container, Address addr, const CacheBlock& accessedBlock) {
        // Increment LRU counters for all blocks in the same set
        auto setBegin = container.setBegin(addr);
        auto setEnd = container.setEnd(addr);
        
        for (auto it = setBegin; it != setEnd; ++it) {
            it->incrementLRU();
        }
        
        accessedBlock.setLRU(0);
    }
};