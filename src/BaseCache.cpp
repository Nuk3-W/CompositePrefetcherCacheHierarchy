#include "Core/BaseCache.h"
#include "Utils/BitUtils.h"

// Using directives for cleaner code
using Config::CacheParams;
using namespace Config::Constants;

BaseCache::BaseCache(const CacheParams& params) :
    params_(params),
    stats_(),
    cache_(params_.sets_ * params_.assoc_) {
    
    // Cache address bits initialization
    int blockBits = static_cast<int>(std::log2(params_.blockSize_));
    int setBits   = static_cast<int>(std::log2(params_.sets_));
    int tagBits   = static_cast<int>(sizeof(Address) * 8 - (blockBits + setBits));

    bitMasks_.offsetBits_ = Utils::makeMask(0, blockBits);
    bitMasks_.setBits_    = Utils::makeMask(blockBits, setBits);
    bitMasks_.tagBits_    = Utils::makeMask(blockBits + setBits, tagBits);
}

AccessResult BaseCache::handleCacheEviction(CacheBlock& block, [[maybe_unused]] Address addr) {
	setValid(block);
    if (isDirtyBlock(block)) {
        return Evict{block.addr_};
    } else {
        return Miss{};
    }
}

void BaseCache::checkHit(bool hit) {
	if (hit) {
		++stats_.hits_;
	} else {
		++stats_.misses_;
	}
}

void BaseCache::updateReadStats(bool hit) { 
	stats_.reads_++;
	checkHit(hit);
	if (!hit) ++stats_.readMiss_;
}

void BaseCache::updateWriteStats(bool hit) { 
	stats_.writes_++;
	checkHit(hit);
	if (!hit) ++stats_.writeMiss_;
}





