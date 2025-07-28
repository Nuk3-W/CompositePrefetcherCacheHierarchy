#pragma once

#include <iostream>
#include <iomanip>
#include <optional>
#include <vector>
#include <cmath>
#include <bitset>

#include "BaseCache.h"
#include "VictimCache.h"

class CacheManager;

class LevelCache : public BaseCache {
public:
    LevelCache(const CacheParams& params, const CacheParams& vParams);

    AccessResult read(Address addr);
    AccessResult write(Address addr);
    void printStats() const;

private:
    enum class AccessType { Read, Write };
    
    AccessResult access(Address addr, AccessType type);
    AccessResult handleHit(Address setIndex, int way, AccessType type);
    AccessResult handleMiss(Address setIndex, Address addr, AccessType type);
    
    AccessResult handleVictim(CacheBlock& evict, Address addr);
    
    std::optional<VictimCache> victimCache_;
};