#include "Caches/LevelCache.h"

LevelCache::LevelCache(CacheParams params) :
	params_( params ),
	stats_ ( ),
	cache_ ( params_.sets_ * params_.assoc_ ),
	extraBits_ ( params_.sets_ * params_.assoc_ ) {
		//cache address bits initialization
		int blockBits = std::log2(params_.blockSize_);
		int setBits = std::log2(params_.sets_);
		int tagBits = sizeof(Address) * 8 - (blockBits + setBits);

		bitMasks_.offsetBits_ = makeMask(0, blockBits);
		bitMasks_.setBits_ = makeMask(blockBits, setBits);
		bitMasks_.tagBits_ = makeMask(blockBits + setBits, tagBits);

		//metadata bits initialization
		int lruBits = std::log2(params_.assoc_);
		int validBits = 1;
		int dirtyBits = 1;

		bitMasks_.lruBits_ = makeMask(0, lruBits);
		bitMasks_.validBits_ = makeMask(lruBits, validBits);
		bitMasks_.dirtyBits_ = makeMask(lruBits + validBits, dirtyBits);
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

additionally update the lru of whichever way is chosen
*/
Address LevelCache::read(Address addr) {
	//contains the set and tag of the address
	DecodedAddress dAddr = decodeAddress(addr);

	for (int i = 0; i < params_.assoc_; ++i) { // (2)
		Address cacheTag = cache_[dAddr.set + i] & bitMasks_.tagBits_;
		Address cacheValid = extraBits_[dAddr.set + i] & bitMasks_.validBits_;

		// skip invalid blocks with no data
		if (!cacheValid) continue; 

		if (cacheTag == dAddr.tag) {
			updateLRU(dAddr.set, i);
			updateReadStats(true); // (3) Cache Hit
			return g_invalidAddress;
		}
	}
	Address victim = getVictimLRU(dAddr.set);
	updateLRU(dAddr.set, victim);
	updateReadStats(false);
	// we need to update the address because it gets put in after the write back
	Address evictedAddr = cache_[dAddr.set + victim];
	// insert new address
	cache_[dAddr.set + victim] = addr;
	extraBits_[dAddr.set + victim] |= bitMasks_.validBits_;

	Address cacheDirty = extraBits_[dAddr.set + victim] & bitMasks_.dirtyBits_;

	if (cacheDirty) {
		updateDirty(dAddr.set, victim);
		return evictedAddr; // (5)
	}

	return addr; // (6)	
}

Address LevelCache::write(Address addr) {

}

Address LevelCache::writeBack(Address addr) { //needs to return either an addr other than the current one or g_invalidAddress

}

LevelCache::DecodedAddress LevelCache::decodeAddress(Address addr) const {
	// Extract set bits and shift by block offset to get set index
	Address setIndex = (addr & bitMasks_.setBits_) >> static_cast<Address>(std::log2(params_.blockSize_));
	// Multiply by assoc_ to get base index in 1D vector
	setIndex *= params_.assoc_;
	// Extract tag bits
	Address tag = addr & bitMasks_.tagBits_;

	return { setIndex, tag };
}

Address LevelCache::getVictimLRU(Address set) const {
	// Find the way with the maximum LRU value in the set
	std::pair<int, int> maxLRUWay = { 0, 0 };
	// Initialize with the first way's LRU value
	for (int i = 0; i < params_.assoc_; ++i) {
		Address cacheLRU = extraBits_[set + i] & bitMasks_.lruBits_;
		// check if the current way has a higher LRU value
		if (cacheLRU > maxLRUWay.second) {
			maxLRUWay = { i, cacheLRU };
		}
	}
	return maxLRUWay.first;
}

void LevelCache::updateLRU(int set, int way) {
	Address checkLRU = extraBits_[set + way] & bitMasks_.lruBits_;

	for (int i = 0; i < params_.assoc_; ++i) {
		Address currentLRU = extraBits_[set + i] & bitMasks_.lruBits_;
		// increment LRU for all blocks with lower LRU value than current
		if (currentLRU < checkLRU) extraBits_[set + i] += 1; 
	}
	// reset the new way in the set
	extraBits_[set + way] &= ~bitMasks_.lruBits_;
}

void LevelCache::updateDirty(int set, int way) {
	extraBits_[set + way] ^= bitMasks_.dirtyBits_;
}

void LevelCache::updateReadStats(bool hit) { // [TODO]
	if (hit) {
		stats_.hits_++;
	} else {
		stats_.misses_++;
	}
}

Address LevelCache::makeMask(int start, int length) const{
	if (length == 0) return 0;
	if (start + length >= sizeof(Address) * 8) return ~0UL << start;
	return ((1UL << length) - 1) << start;
}
