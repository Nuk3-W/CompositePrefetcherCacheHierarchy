#include "Caches/LevelCache.h"

LevelCache::LevelCache(const CacheParams& params, const CacheParams& vParams) :
	BaseCache(params),
	victimCache_(vParams.size_ > 0 ? std::make_optional<VictimCache>(vParams) : std::nullopt)
{}

Address LevelCache::read(Address addr) { 
	DecodedAddress dAddr = decodeAddress(addr);

	int hitIndex = findInSet(dAddr);
	if (hitIndex != -1){
		updateLRU(dAddr.set, hitIndex);
		updateReadStats(true); 
		return g_invalidAddress;
	}

	int victimIndex = getVictimLRU(dAddr.set);
	CacheBlock& victimBlock = cache_[dAddr.set + victimIndex];

	updateLRU(dAddr.set, victimIndex);
	Address evictedAddr{};
	if (victimCache_){
		//evictedAddr = handleVictim(dAddr, victimIndex, addr);
	} else {
		evictedAddr = handleCacheEviction(victimBlock, addr);
	}

	cache_[dAddr.set + victimIndex].addr_ = addr;

	clearDirty(victimBlock);

	updateReadStats(false);

	return evictedAddr;
}

Address LevelCache::write(Address addr) { //possible change for more clear code just always reset the dirty bit and let the caller change back the dirty bit so its less for read
	DecodedAddress dAddr = decodeAddress(addr);

	int hitIndex = findInSet(dAddr);
	if ( hitIndex != -1 ){
		setDirty(cache_[dAddr.set + hitIndex]);
		updateLRU(dAddr.set, hitIndex);
		updateWriteStats(true);
		return g_invalidAddress;
	}

	int victimIndex = getVictimLRU(dAddr.set);
	CacheBlock& victimBlock = cache_[dAddr.set + victimIndex];

	Address evictedAddr;
	if (victimCache_){
		evictedAddr = handleVictim(victimBlock, addr);
	} else {
		evictedAddr = handleCacheEviction(victimBlock, addr);
	}

	victimBlock.addr_ = addr;

	updateLRU(dAddr.set, victimIndex);
	setDirty(victimBlock);

	updateWriteStats(false);

	return evictedAddr;
}

Address LevelCache::handleVictim(CacheBlock& evict, Address addr) {
	Address victimBlock = victimCache_->read(addr);
	BaseCache::CacheBlock& swapEvict = victimCache_->write(evict.addr_, static_cast<bool>( evict.extraBits_ & bitMasks_.dirtyBits_ ));
	if (victimBlock == g_invalidAddress) {
		evict.addr_ = swapEvict.addr_;
	} else if (victimBlock == addr) {
		
	} else {

	}

	return 0;
}

void LevelCache::printStats() const {
	std::cout << "Reads: " << stats_.reads_ << "\n"
		<< "Read Misses: " << stats_.readMiss_ << "\n"
		<< "Writes: " << stats_.writes_ << "\n"
		<< "Write Misses: " << stats_.writeMiss_ << "\n"
		<< "Hits: " << stats_.hits_ << "\n"
		<< "Misses: " << stats_.misses_ << "\n";
}