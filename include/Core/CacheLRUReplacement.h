#pragma once

#include "Core/CacheContainer.h"
#include "Core/CacheBlock.h"
#include "Core/Types.h"

using LRUValue = uint8_t;

template<typename Block>
class CacheLRUReplacement {
public:
    using CacheContainerT = CacheContainer<Block>;
    using BlockRef = std::reference_wrapper<Block>;

    CacheLRUReplacement() = default;
    ~CacheLRUReplacement() = default;

    BlockRef evict(CacheContainerT& container, Address addr) {
        auto setBegin = container.setBegin(addr);
        auto setEnd = container.setEnd(addr);
        
        auto leastRecentlyUsedBlock = setBegin;
        LRUValue highestLRUValue = leastRecentlyUsedBlock->getLRU();
        
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
    
    void updateLRU(CacheContainerT& container, Address addr, Block& accessedBlock) {
        const LRUValue accessedBlockLRU = accessedBlock.getLRU();
        for (auto currentBlock = container.setBegin(addr); currentBlock != container.setEnd(addr); ++currentBlock) {
            if (currentBlock->getLRU() <= accessedBlockLRU) {
                currentBlock->incrementLRU();
            }
        }
        
        accessedBlock.setLRU(static_cast<LRUValue>(0));
    }
};