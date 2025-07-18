#ifndef LevelCache_H
#define LevelCache_H

#include <iostream>
#include <optional>
#include <vector>
#include <cmath>
#include <bitset>
#include <iomanip>
#include <optional>

#include "BaseCache.h"
#include "VictimCache.h"

class CacheManager;

class LevelCache : public BaseCache {
public:
	friend class CacheManager;
	LevelCache(const CacheParams& params, const CacheParams& vParams);
	~LevelCache() = default;

	Address read(Address addr);
	Address write(Address addr);
	void printStats() const;
private:
	enum class AccessType { Read, Write };
	Address access(Address addr, AccessType type);
	Address handleVictim(CacheBlock& evict, Address addr);
	std::optional<VictimCache> victimCache_;
};
#endif // LevelCache_H