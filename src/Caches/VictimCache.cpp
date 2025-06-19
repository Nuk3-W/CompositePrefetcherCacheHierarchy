#include "Caches/VictimCache.h"

VictimCache::VictimCache(const CacheParams& params) :
	LevelCache(params) {
	valid = params.size_ == 0 ? false : true;
}

