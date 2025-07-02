#ifndef VICTIM_CACHE_H
#define VICTIM_CACHE_H

#include <iostream>
#include <utility>

#include "Caches/BaseCache.h"
#include "Caches/CacheData.h"

class VictimCache : public BaseCache {
public:
	friend class LevelCache;
	VictimCache(const CacheParams& params);
	~VictimCache() = default;

	Address VictimCache::read(Address addr);
	BaseCache::CacheBlock VictimCache::write(Address addr, bool dirty);
	void printStats() const;

private:
	CacheBlock& swap();
};

#endif // VICTIM_CACHE_H