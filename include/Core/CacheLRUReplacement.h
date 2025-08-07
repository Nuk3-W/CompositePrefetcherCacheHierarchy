#pragma once

#include "Config/CacheParams.h"
#include "Core/CacheContainer.h"
#include "Core/CacheBlock.h"
#include "Core/Types.h"

using LRUValue = uint8_t;
using CacheBlockRef = std::reference_wrapper<CacheBlock>;

class CacheLRUReplacement {
public:
    CacheLRUReplacement() = default;
    ~CacheLRUReplacement() = default;

    CacheBlockRef evict(CacheContainer& container, Address addr) {
        auto setBegin = container.setBegin(addr);
        auto setEnd = container.setEnd(addr);
        
        auto leastRecentlyUsedBlock = setBegin;
        LRUValue highestLRUValue = leastRecentlyUsedBlock->getLRU();
        
        // find LRU block or invalid block prefer invalid block
        for (auto currentBlock = setBegin; currentBlock != setEnd; ++currentBlock) {
            if (!currentBlock->isValid()) {
                updateLRU(container, addr, *currentBlock);
                return *currentBlock;
            }
            if (currentBlock->getLRU() > highestLRUValue) {
                highestLRUValue = currentBlock->getLRU();
                leastRecentlyUsedBlock = currentBlock;
            }
        }
        
        updateLRU(container, addr, *leastRecentlyUsedBlock);
        return *leastRecentlyUsedBlock;
    }
    
    void updateLRU(CacheContainer& container, Address addr, CacheBlock& accessedBlock) {
        const LRUValue accessedBlockLRU = accessedBlock.getLRU();
        for (auto currentBlock = container.setBegin(addr); currentBlock != container.setEnd(addr); ++currentBlock) {
            if (currentBlock->getLRU() <= accessedBlockLRU) {
                currentBlock->incrementLRU();
            }
        }
        
        accessedBlock.setLRU(static_cast<LRUValue>(0));
    }
};