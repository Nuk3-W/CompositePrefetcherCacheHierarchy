#pragma once

#include <vector>
#include <cmath>
#include <iostream>
#include <bitset>
#include <iomanip>
#include <optional>

#include "Types.h"
#include "Config/Constants.h"
#include "Config/CacheParams.h"

// Using directives for cleaner code
using Config::CacheParams;
using Config::CacheStats;

class CacheManager;

class BaseCache {
public:
    BaseCache(const CacheParams& params);
    ~BaseCache() = default;

    void printStats() const;
    
protected:
    struct CacheBlock;

    std::optional<int> findHitWay(Address addr, Address& setIndex) const;

    int getVictimLRU(Address set) const;
    void updateLRU(int set, int way);
    
    
    AccessResult handleCacheEviction(CacheBlock&, Address newAddr);
    
    void updateReadStats(bool hit);
    void updateWriteStats(bool hit);
    void checkHit(bool hit);
    

    
protected:
    struct CacheBlock {
        Address addr_{};
        uint32_t extraBits_{};
    };
    
    struct BitMasks {
        Address tagBits_{};
        Address setBits_{};
        Address offsetBits_{};
    };
    
    BitMasks bitMasks_{};
    CacheParams params_{};
    CacheStats stats_{};
    std::vector<CacheBlock> cache_{};
};