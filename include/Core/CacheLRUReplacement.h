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
        uint8_t maxLRU = lruBlock->getLRU();
        
        for (auto it = setBegin; it != setEnd; ++it) {
            if (!it->isValid()) {
                updateLRU(container, addr, *it);
                return *it;  // Return invalid block immediately
            }
            if (it->getLRU() > maxLRU) {
                maxLRU = it->getLRU();
                lruBlock = it;
            }
        }
        
        updateLRU(container, addr, *lruBlock);
        return *lruBlock;
    }
    
    void updateLRU(CacheContainer& container, Address addr, CacheBlock& accessedBlock) {
        const auto maxLRU = accessedBlock.getLRU();
        for (auto it = container.setBegin(addr); it != container.setEnd(addr); ++it) {
            if (it->getLRU() <= maxLRU) {
                it->incrementLRU();
            }
        }
        
        accessedBlock.setLRU(static_cast<uint8_t>(0));
    }
};