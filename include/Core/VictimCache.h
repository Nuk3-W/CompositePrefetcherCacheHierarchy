#pragma once

#include <iostream>
#include <utility>

#include "BaseCache.h"
#include "Config/CacheParams.h"

// Using directives for cleaner code
using Config::CacheParams;

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