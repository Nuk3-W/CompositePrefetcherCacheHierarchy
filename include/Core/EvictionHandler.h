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
        
        // if writing back to Ram/MainMemory still need to record writeback
        StatisticsManager::getInstance().recordWriteback(fromLevel);
        
        if (fromLevel + 1 >= levels.size()) return;
        
        Address evictedAddress = Utils::getAddress(eviction);
        
        AccessResult nextLevelResult = levels[fromLevel + 1].write(evictedAddress);

        if (Utils::isType<Hit>(nextLevelResult)) {
            Utils::getBlock(nextLevelResult).copy(Utils::getBlock(eviction));
            return;
        } else if (Utils::isType<Evict>(nextLevelResult)) {
            processEviction(nextLevelResult, fromLevel + 1, levels);
        }

        // dont read what we just written to
        traverseCacheHierarchy(evictedAddress, fromLevel + 2, levels);
    
        Utils::getBlock(nextLevelResult).copy(Utils::getBlock(eviction));
    }

private:
    void setupBlock(AccessResult& result, Address address) {
        Utils::getBlock(result).initialize(address, AccessType::Read);
    }
    
    void traverseCacheHierarchy(Address addr, std::size_t startLevel, std::vector<LevelCache>& levels) {
        for (std::size_t i = startLevel; i < levels.size(); ++i) {
            AccessResult result = levels[i].read(addr);
            if (Utils::isType<Hit>(result)) return;
            if (Utils::isType<Evict>(result)) {
                processEviction(result, i, levels);
            }
            setupBlock(result, addr);
        }
    }
};