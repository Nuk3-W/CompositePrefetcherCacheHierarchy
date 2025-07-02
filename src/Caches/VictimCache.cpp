#include "Caches/VictimCache.h"

VictimCache::VictimCache(const CacheParams& params) : BaseCache(params) {}

Address VictimCache::read(Address addr){
	return 0;
}

BaseCache::CacheBlock VictimCache::write(Address addr, bool dirty) {
	return CacheBlock{};
}