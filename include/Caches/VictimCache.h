#pragma once

#include <iostream>
#include <utility>

#include "Caches/BaseCache.h"
#include "Caches/CacheData.h"

class VictimCache : public BaseCache {
public:
	VictimCache(const CacheParams& params);
	~VictimCache() = default;

	AccessResult swapReq(CacheBlock& block, Address addr);
	void printStats() const;
private:
	AccessResult swap(CacheBlock& block, int hitWay, Address setIndex);
	AccessResult insertBlock(CacheBlock& block, Address setIndex, Address addr);
};