#include "Caches/LevelCache.h"

LevelCache::LevelCache(const CacheParams& params) : BaseCache(params) {}

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

void LevelCache::printStats() const {
	std::cout << "Reads: " << stats_.reads_ << "\n"
		<< "Read Misses: " << stats_.readMiss_ << "\n"
		<< "Writes: " << stats_.writes_ << "\n"
		<< "Write Misses: " << stats_.writeMiss_ << "\n"
		<< "Hits: " << stats_.hits_ << "\n"
		<< "Misses: " << stats_.misses_ << "\n";
}