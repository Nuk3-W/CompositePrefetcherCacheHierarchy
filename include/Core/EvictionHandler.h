#pragma once

#include "Core/Types.h"
#include "Core/LevelCache.h"
#include "Core/StatisticsManager.h"
#include "Utils/VariantUtils.h"
#include <vector>

class EvictionHandler {
public:
    EvictionHandler() = default;
    ~EvictionHandler() = default;

    void processEviction(AccessResult eviction, std::size_t fromLevel, std::vector<LevelCache>& levels) {
        if (!Utils::isType<Evict>(eviction)) return;
        
        StatisticsManager::getInstance().recordWriteback(fromLevel);
        
        if (fromLevel + 1 >= levels.size()) return;
        
        Address addr = Utils::getAddress(eviction);
        
        // Write evicted block to next level
        AccessResult result = levels[fromLevel + 1].write(addr);

        if (Utils::isType<Hit>(result)) {
            Utils::getBlock(result).copy(Utils::getBlock(eviction));
            return;
        } else if (Utils::isType<Evict>(result)) {
            processEviction(result, fromLevel + 1, levels);
        }

        // we already read level + 1, so we need to read level + 2
        traverseCacheHierarchy(addr, fromLevel + 2, levels);
    
        Utils::getBlock(result).copy(Utils::getBlock(eviction));
    }

private:
    void traverseCacheHierarchy(Address addr, std::size_t startLevel, std::vector<LevelCache>& levels) {
        for (std::size_t i = startLevel; i < levels.size(); ++i) {
            AccessResult result = levels[i].read(addr);
            if (Utils::isType<Hit>(result)) return;
            if (Utils::isType<Evict>(result)) {
                processEviction(result, i, levels);
            }
            Utils::getBlock(result).setAddress(addr);
            Utils::getBlock(result).setValid();
            Utils::getBlock(result).clearDirty();
        }
    }
};