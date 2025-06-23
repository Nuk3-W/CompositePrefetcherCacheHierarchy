#ifndef LevelCache_H
#define LevelCache_H

#include <iostream>
#include <vector>
#include <cmath>
#include <bitset>
#include <iomanip>

#include "BaseCache.h"

class CacheManager;

class LevelCache : public BaseCache {
public:
	friend class CacheManager;
	LevelCache(const CacheParams& params);
	~LevelCache() = default;

	Address read(Address addr); 
	Address write(Address addr);
	void printStats() const;
protected:
	VictimCache vCache;
};
#endif // LevelCache_H