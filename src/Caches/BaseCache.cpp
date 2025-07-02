#include "Caches/BaseCache.h"

BaseCache::BaseCache(const CacheParams& params) :
	params_(params),
	stats_ (),
	cache_ (params_.sets_* params_.assoc_) {
	//cache address bits initialization
	int blockBits = std::log2(params_.blockSize_);
	int setBits = std::log2(params_.sets_);
	int tagBits = sizeof(Address) * 8 - ( blockBits + setBits );

	bitMasks_.offsetBits_ = makeMask(0, blockBits);
	bitMasks_.setBits_ = makeMask(blockBits, setBits);
	bitMasks_.tagBits_ = makeMask(blockBits + setBits, tagBits);

	std::cout << "=== Address Bit Masks ===\n";
	printMask("Offset", 0, blockBits, bitMasks_.offsetBits_);
	printMask("Set", blockBits, setBits, bitMasks_.setBits_);
	printMask("Tag", blockBits + setBits, tagBits, bitMasks_.tagBits_);

	//metadata bits initialization
	int lruBits = std::log2(params_.assoc_);
	int validBits = 1;
	int dirtyBits = 1;

	bitMasks_.lruBits_ = makeMask(0, lruBits);
	bitMasks_.validBits_ = makeMask(lruBits, validBits);
	bitMasks_.dirtyBits_ = makeMask(lruBits + validBits, dirtyBits);

	std::cout << "\n=== Metadata Bit Masks ===\n";
	printMask("LRU", 0, lruBits, bitMasks_.lruBits_);
	printMask("Valid", lruBits, validBits, bitMasks_.validBits_);
	printMask("Dirty", lruBits + validBits, dirtyBits, bitMasks_.dirtyBits_);
}

int BaseCache::findInSet(const DecodedAddress& dAddr) const {
	for (int i = 0; i < params_.assoc_; ++i){
		int cacheIndex = dAddr.set + i;

		// skip invalid blocks
		if (!isValidBlock(cache_[cacheIndex])) continue;

		Address cacheTag = cache_[cacheIndex].addr_ & bitMasks_.tagBits_;
		if (cacheTag == dAddr.tag) return i;

		//std::cout << "debug in findInSet" << std::endl;
	}
	//not found in cache
	return -1;
}


int BaseCache::getVictimLRU(Address set) const {
	// Find the way with the maximum LRU value in the set
	std::pair<int, int> maxLRUWay = { 0, 0 };
	// Initialize with the first way's LRU value 
	for (int i = 0; i < params_.assoc_; ++i) {
		Address cacheLRU = cache_[set + i].extraBits_ & bitMasks_.lruBits_;
		// check if the current way has a higher LRU value or if invalid because they are used first
		if (!isValidBlock(cache_[set + i])){
			return i;
		}
		else if (cacheLRU > maxLRUWay.second) {
			maxLRUWay = { i, cacheLRU };
		}
	}
	return maxLRUWay.first;
}

void BaseCache::updateLRU(int set, int way) {
	Address checkLRU = cache_[set + way].extraBits_ & bitMasks_.lruBits_;

	for (int i = 0; i < params_.assoc_; ++i) {
		Address currentLRU = cache_[set + i].extraBits_ & bitMasks_.lruBits_;
		// increment LRU for all blocks with lower LRU value than current
		if (currentLRU <= checkLRU && currentLRU < bitMasks_.lruBits_) cache_[set + i].extraBits_ += 1;
	}
	// reset the use block to 0
	cache_[set + way].extraBits_ &= ~bitMasks_.lruBits_;
}

Address BaseCache::handleCacheEviction(CacheBlock& block, Address newAddr) {

	Address evictedAddr = block.addr_;

	//cache_[cacheIndex].addr_ = newAddr;
	setValid(block);

	if (isDirtyBlock(block)) {
		return evictedAddr;
	}

	return newAddr;
}

BaseCache::DecodedAddress BaseCache::decodeAddress(Address addr) const {
	// Extract set bits and shift by block offset to get set index
	Address setIndex = ( addr & bitMasks_.setBits_ ) >> static_cast< Address >( std::log2(params_.blockSize_) );
	// Multiply by assoc_ to get base index in 1D vector
	setIndex *= params_.assoc_;
	// Extract tag bits
	Address tag = addr & bitMasks_.tagBits_;

	return { tag, setIndex };
}

void BaseCache::checkHit(bool hit) {
	if (hit) {
		++stats_.hits_;
	}
	else {
		++stats_.misses_;
	}
}

void BaseCache::updateReadStats(bool hit) { // [TODO]
	stats_.reads_++;
	checkHit(hit);
	if (!hit) ++stats_.readMiss_;
}

void BaseCache::updateWriteStats(bool hit) { // [TODO]
	stats_.writes_++;
	checkHit(hit);
	if (!hit) ++stats_.writeMiss_;
}

Address BaseCache::makeMask(int start, int length) const {
	if (length == 0) return 0;
	if (start + length >= sizeof(Address) * 8) return ~0UL << start;
	return ( ( 1UL << length ) - 1 ) << start;
}

//Asccessor and Mutator Functions for dirty and valid bits
bool BaseCache::isDirtyBlock(const CacheBlock& block) const {
	return block.extraBits_ & bitMasks_.dirtyBits_;
}

void BaseCache::setDirty(CacheBlock& block) {
	block.extraBits_ |= bitMasks_.dirtyBits_;
}

void BaseCache::clearDirty(CacheBlock& block) {
	block.extraBits_ &= ~bitMasks_.dirtyBits_;
}

bool BaseCache::isValidBlock(const CacheBlock& block) const {
	return block.extraBits_ & bitMasks_.validBits_;
}

void BaseCache::setValid(CacheBlock& block) {
	block.extraBits_ |= bitMasks_.validBits_;
}
// ------------------------------------------------------

void BaseCache::printMask(const std::string& label, int start, int length, unsigned long mask) const {
	std::cout << std::left << std::setw(12) << label
		<< "Start: " << std::setw(2) << start
		<< " Len: " << std::setw(2) << length
		<< " Hex: 0x" << std::hex << std::setw(16) << mask
		<< "  Bin: " << std::bitset<32>(mask)
		<< std::dec << '\n';
}



