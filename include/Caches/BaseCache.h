#ifndef BaseCache_H
#define BaseCache_H

#include <vector>
#include <cmath>
#include <iostream>
#include <bitset>
#include <iomanip>
#include <optional>

#include "CacheData.h"

class CacheManager;

// global metadata bitmasks for all caches
constexpr int g_reservedLRUBits = 8;
constexpr int g_validBits = 1;
constexpr int g_dirtyBits = 1;
constexpr Address g_LRU_MASK   = ((1UL << g_reservedLRUBits) - 1) << 0;
constexpr Address g_VALID_MASK = (1UL << g_reservedLRUBits);
constexpr Address g_DIRTY_MASK = (1UL << (g_reservedLRUBits + 1));

class BaseCache {
public:
	// can be defined because all caches must have these params even if victim
	BaseCache(const CacheParams& params);
	~BaseCache() = default;

	// I dont want virtual functions here to avoid vtable overhead so I leave undefined for linker errors to act as pure virtuals
	// and yes the performance loss isn't much but I decided to be strict about it anyways fight me
	Address read(Address addr);
	Address write(Address addr);
	void printStats() const;
protected:
    struct CacheBlock;

    int getVictimLRU(Address set) const;
    void updateLRU(int set, int way);
    
	bool isValidBlock(const CacheBlock& block) const;
    void setValid(CacheBlock& block);
    
	bool isDirtyBlock(const CacheBlock& block) const;
    void setDirty(CacheBlock& block);
    void clearDirty(CacheBlock& block);
    
	Address handleCacheEviction(CacheBlock&, Address newAddr);
    
	void updateReadStats(bool hit);
    void updateWriteStats(bool hit);
    void checkHit(bool hit);
    
	Address makeMask(int start, int length) const;
    void printMask(const std::string& label, int start, int length, unsigned long mask) const;
    
	std::optional<int> findHitWay(Address addr, Address& setIndex) const;

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