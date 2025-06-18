#include "Caches/VictimCache.h"

VictimCache::VictimCache(const CacheParams& params) :
	LevelCache(params) {
	valid = params.size_ == 0 ? false : true;
}

Address VictimCache::read(Address addr){
	return 0;
}

Address VictimCache::write(Address addr) {
	return 0;
}

Address VictimCache::writeBack(Address addr) {
	return 0;
}

bool VictimCache::isValid() const {
	return valid;
}
