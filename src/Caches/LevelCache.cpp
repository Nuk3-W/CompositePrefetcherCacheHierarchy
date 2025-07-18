#include "Caches/LevelCache.h"

LevelCache::LevelCache(const CacheParams& params, const CacheParams& vParams) :
	BaseCache(params),
	victimCache_(vParams.size_ > 0 ? std::make_optional<VictimCache>(vParams) : std::nullopt)
{}

// Removed findHitWay from LevelCache.cpp; now implemented in BaseCache.cpp

Address LevelCache::access(Address addr, AccessType type) {
	Address setIndex;
	auto hitWay = findHitWay(addr, setIndex);
	if (hitWay) {
		if (type == AccessType::Write) {
			setDirty(cache_[setIndex + *hitWay]);
			updateWriteStats(true);
		} else {
			updateReadStats(true);
		}
		updateLRU(setIndex, *hitWay);
		return g_cacheHitAddress;
	}

	int victimIndex = getVictimLRU(setIndex);
	CacheBlock& victimBlock = cache_[setIndex + victimIndex];
	Address evictedAddr{};
	if (victimCache_) {
		evictedAddr = handleVictim(victimBlock, addr); // TODO: update if needed
	} else {
		evictedAddr = handleCacheEviction(victimBlock, addr);
	}
	cache_[setIndex + victimIndex].addr_ = addr;
	updateLRU(setIndex, victimIndex);
	if (type == AccessType::Write) {
		setDirty(victimBlock);
		updateWriteStats(false);
	} else {
		clearDirty(victimBlock);
		updateReadStats(false);
	}
	return evictedAddr;
}

Address LevelCache::read(Address addr) {
	return access(addr, AccessType::Read);
}

Address LevelCache::write(Address addr) {
	return access(addr, AccessType::Write);
}

Address LevelCache::handleVictim(CacheBlock& evict, Address addr) {
	Address victimBlock = victimCache_->read(addr);
	BaseCache::CacheBlock& swapEvict = victimCache_->write(evict.addr_, static_cast<bool>( evict.extraBits_ & bitMasks_.dirtyBits_ ));
	if (victimBlock == g_cacheHitAddress) {
		evict.addr_ = swapEvict.addr_;
	} else if (victimBlock == addr) {
		// No-op for now
	} else {
		// No-op for now
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