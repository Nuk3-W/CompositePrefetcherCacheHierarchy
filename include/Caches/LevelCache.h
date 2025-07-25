#ifndef LEVEL_CACHE_H
#define LEVEL_CACHE_H

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

    Address read(Address addr);
    Address write(Address addr);
    void printStats() const;

private:
    enum class AccessType { Read, Write };
    
    Address access(Address addr, AccessType type);
    Address handleHit(Address setIndex, int way, AccessType type);
    Address handleMiss(Address setIndex, Address addr, AccessType type);
    
    Address handleVictim(CacheBlock& evict, Address addr);
    
    std::optional<VictimCache> victimCache_;
};
#endif // LEVEL_CACHE_H