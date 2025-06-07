#include <vector>

#include "Caches/LevelCache.h"

LevelCache::LevelCache(CacheParams params) :
	params_{ params },
	stats_ { },
	cache_ { params_.sets_ * params_.assoc_ }
	{}

LevelCache::~LevelCache() = default;

Address LevelCache::read(Address addr) {

}

Address LevelCache::write(Address addr) {

}

Address LevelCache::writeBack(Address addr) { //needs to return either an addr other than the current one or g_invalidAddress

}
