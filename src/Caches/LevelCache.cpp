#include "Caches/LevelCache.h"

LevelCache::LevelCache(const CacheParams& params) :
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

Address LevelCache::read(Address addr) { 
	DecodedAddress dAddr = decodeAddress(addr);

	int hitIndex = findInSet(dAddr);
	if (hitIndex != -1){
		updateLRU(dAddr.set, hitIndex);
		updateReadStats(true); 
		return g_invalidAddress;
	}

	int victimIndex = getVictimLRU(dAddr.set);
	updateLRU(dAddr.set, victimIndex);
	Address evictedAddr = handleCacheEviction(dAddr, victimIndex, addr);

	clearDirty(dAddr.set, victimIndex);

	updateReadStats(false);

	return evictedAddr;
}

Address LevelCache::write(Address addr) { //possible change for more clear code just always reset the dirty bit and let the caller change back the dirty bit so its less for read
	DecodedAddress dAddr = decodeAddress(addr);

	int hitIndex = findInSet(dAddr);
	if ( hitIndex != -1 ){
		setDirty(dAddr.set, hitIndex);
		updateLRU(dAddr.set, hitIndex);
		updateWriteStats(true);
		return g_invalidAddress;
	}

	int victimIndex = getVictimLRU(dAddr.set);

	Address evictedAddr = handleCacheEviction(dAddr, victimIndex, addr);

	updateLRU(dAddr.set, victimIndex);
	setDirty(dAddr.set, victimIndex);

	updateWriteStats(false);

	return evictedAddr;
}

int LevelCache::findInSet(const DecodedAddress& dAddr) const {
	for (int i = 0; i < params_.assoc_; ++i){
		int cacheIndex = dAddr.set + i;

		// skip invalid blocks
		if (!isValidBlock(cacheIndex)) continue; 

		Address cacheTag = cache_[cacheIndex] & bitMasks_.tagBits_;
		if (cacheTag == dAddr.tag) return i;

		//std::cout << "debug in findInSet" << std::endl;
	}
	//not found in cache
	return -1;
}


int LevelCache::getVictimLRU(Address set) const {
	// Find the way with the maximum LRU value in the set
	std::pair<int, int> maxLRUWay = { 0, 0 };
	// Initialize with the first way's LRU value
	for ( int i = 0; i < params_.assoc_; ++i ) {
		Address cacheLRU = extraBits_[set + i] & bitMasks_.lruBits_;
		// check if the current way has a higher LRU value
		if ( cacheLRU > maxLRUWay.second ) {
			maxLRUWay = { i, cacheLRU };
		}
	}
	return maxLRUWay.first;
}

void LevelCache::updateLRU(int set, int way) {
	Address checkLRU = extraBits_[set + way] & bitMasks_.lruBits_;

	for ( int i = 0; i < params_.assoc_; ++i ) {
		Address currentLRU = extraBits_[set + i] & bitMasks_.lruBits_;
		// increment LRU for all blocks with lower LRU value than current
		if ( currentLRU < checkLRU ) extraBits_[set + i] += 1;
	}
	// reset the evicted block's LRU bits to 0
	extraBits_[set + way] &= ~bitMasks_.lruBits_;
}

Address LevelCache::handleCacheEviction(const DecodedAddress& dAddr, int victimIndex, Address newAddr) {
	int cacheIndex = dAddr.set + victimIndex;

	Address evictedAddr = cache_[cacheIndex];

	cache_[cacheIndex] = newAddr;
	setValid(dAddr.set, victimIndex);

	if (isDirtyBlock(cacheIndex)) {
		return evictedAddr; 
	}

	return newAddr;
}

LevelCache::DecodedAddress LevelCache::decodeAddress(Address addr) const {
	// Extract set bits and shift by block offset to get set index
	Address setIndex = (addr & bitMasks_.setBits_) >> static_cast<Address>(std::log2(params_.blockSize_));
	// Multiply by assoc_ to get base index in 1D vector
	setIndex *= params_.assoc_;
	// Extract tag bits
	Address tag = addr & bitMasks_.tagBits_;

	return { tag, setIndex };
}

void LevelCache::updateReadStats(bool hit) { // [TODO]
	if (hit) {
		stats_.hits_++;
	} else {
		stats_.misses_++;
	}
}

void LevelCache::updateWriteStats(bool hit) { // [TODO]
	if (hit) {
		stats_.hits_++;
	}
	else {
		stats_.misses_++;
	}
}

void LevelCache::updateWriteBackStats(bool hit) { // [TODO]
	if (hit) {
		stats_.hits_++;
	}
	else {
		stats_.misses_++;
	}
}

Address LevelCache::makeMask(int start, int length) const{
	if (length == 0) return 0;
	if (start + length >= sizeof(Address) * 8) return ~0UL << start;
	return ((1UL << length) - 1) << start;
}

//Asccessor and Mutator Functions for dirty and valid bits
bool LevelCache::isDirtyBlock(int cacheIndex) const {
	return extraBits_[cacheIndex] & bitMasks_.dirtyBits_;
}

void LevelCache::setDirty(int set, int way) {
	extraBits_[set + way] |= bitMasks_.dirtyBits_;
}

void LevelCache::clearDirty(int set, int way) {
	extraBits_[set + way] &= ~bitMasks_.dirtyBits_;
}

bool LevelCache::isValidBlock(int cacheIndex) const {
	return extraBits_[cacheIndex] & bitMasks_.validBits_;
}

void LevelCache::setValid(int set, int way) {
	extraBits_[set + way] |= bitMasks_.validBits_;
}
// ------------------------------------------------------

