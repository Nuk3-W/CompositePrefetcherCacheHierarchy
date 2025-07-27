#ifndef BASE_CACHE_H
#define BASE_CACHE_H

#include <vector>
#include <cmath>
#include <iostream>
#include <bitset>
#include <iomanip>
#include <optional>

#include "CacheData.h"
#include "SystemCacheData.h"

class CacheManager;

// Global metadata bitmasks for all caches
constexpr int g_reservedLruBits = 8;
constexpr int g_validBits = 1;
constexpr int g_dirtyBits = 1;
constexpr Address g_lruMask   = ((1UL << g_reservedLruBits) - 1) << 0;
constexpr Address g_validMask = (1UL << g_reservedLruBits);
constexpr Address g_dirtyMask = (1UL << (g_reservedLruBits + 1));

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
    
    bool isValidBlock(const CacheBlock& block) const;
    void setValid(CacheBlock& block);
    
    bool isDirtyBlock(const CacheBlock& block) const;
    void setDirty(CacheBlock& block);
    void clearDirty(CacheBlock& block);
    
    AccessResult handleCacheEviction(CacheBlock&, Address newAddr);
    
    void updateReadStats(bool hit);
    void updateWriteStats(bool hit);
    void checkHit(bool hit);
    
    Address makeMask(int start, int length) const;
    void printMask(const std::string& label, int start, int length, unsigned long mask) const;
    
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
#endif // BaseCache_H