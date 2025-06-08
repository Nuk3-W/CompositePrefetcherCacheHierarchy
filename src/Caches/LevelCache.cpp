#include "Caches/LevelCache.h"

LevelCache::LevelCache(CacheParams params) :
	params_( params ),
	stats_ ( ),
	cache_ ( params_.sets_ * params_.assoc_ ),
	extraBits_ ( params_.sets_ * params_.assoc_ ) {
		int blockBits = std::log2(params_.blockSize_);
		int setBits = std::log2(params_.sets_);
		int addressBits = sizeof(Address) * 8;

		offsetBits_ = makeMask(0, blockBits);
		setBits_ = makeMask(blockBits, setBits);
		tagBits_ = makeMask(blockBits + setBits, addressBits - (blockBits + setBits));
	}

LevelCache::~LevelCache() = default;


/*
Read function structure:
	(1) use bitmask to check correct set
	(2) iterate through set to find matching tag
	(3) if found, return g_invalidAddress and update stats as cache hit
	(4) if not found, find maxLRU block in set
	(5) if dirty, return the blocks address for write-back
	(6) if not dirty, return addr
*/
Address LevelCache::read(Address addr) {
	using bitMask = unsigned long;

	bitMask addrSet = addr & setBits_; // (1)
	bitMask addrTag = addr & tagBits_;
	addrSet >>= static_cast<bitMask>(std::log2(params_.blockSize_)); // shift to get the correct index for the set

	int maxLRUWay = -1;
	for (int i = 0; i < params_.assoc_; ++i) { // (2)
		bitMask cacheTag = cache_[addrSet + i] & tagBits_; 

		if (cacheTag == addrTag) {
			updateReadStats(true); // (3) Cache Hit
			return g_invalidAddress;
		}





	}
}

Address LevelCache::write(Address addr) {

}

Address LevelCache::writeBack(Address addr) { //needs to return either an addr other than the current one or g_invalidAddress

}

void LevelCache::updateReadStats(bool hit) { // [TODO]
	if (hit) {
		stats_.hits_++;
	} else {
		stats_.misses_++;
	}
}

Address LevelCache::makeMask(int start, int length) {
	if (length == 0) return 0;
	if (start + length >= sizeof(Address) * 8) return ~0UL << start;
	return ((1UL << length) - 1) << start;
}
