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
        
        // Prepare hierarchy
        for (std::size_t i = fromLevel + 1; i < levels.size(); ++i) {
            AccessResult result = levels[i].read(addr);
            if (Utils::isType<Hit>(result)) break;
            if (Utils::isType<Evict>(result)) {
                processEviction(result, i, levels);
            }
            Utils::getBlock(result).setAddress(addr);
            Utils::getBlock(result).setValid();
            Utils::getBlock(result).clearDirty();
        }
        
        // Write to next level
        AccessResult result = levels[fromLevel + 1].write(addr);
        Utils::getBlock(result).copy(Utils::getBlock(eviction));
    }
};