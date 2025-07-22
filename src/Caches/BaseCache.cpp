#include "Caches/BaseCache.h"
#include <optional>

BaseCache::BaseCache(const CacheParams& params) :
    params_(params),
    stats_(),
    cache_(params_.sets_ * params_.assoc_) {
    
    // Cache address bits initialization
    int blockBits = static_cast<int>(std::log2(params_.blockSize_));
    int setBits   = static_cast<int>(std::log2(params_.sets_));
    int tagBits   = static_cast<int>(sizeof(Address) * 8 - (blockBits + setBits));

    bitMasks_.offsetBits_ = makeMask(0, blockBits);
    bitMasks_.setBits_    = makeMask(blockBits, setBits);
    bitMasks_.tagBits_    = makeMask(blockBits + setBits, tagBits);
}

std::optional<int> BaseCache::findHitWay(Address addr, Address& setIndex) const {
    int blockBits = static_cast<int>(std::log2(params_.blockSize_));
    setIndex = ((addr & bitMasks_.setBits_) >> blockBits) * params_.assoc_;
    Address tag = addr & bitMasks_.tagBits_;
    
    for (std::size_t i = 0; i < params_.assoc_; ++i) {
        int cacheIndex = static_cast<int>(setIndex + i);
        if (!isValidBlock(cache_[cacheIndex])) continue;
        Address cacheTag = cache_[cacheIndex].addr_ & bitMasks_.tagBits_;
        if (cacheTag == tag) return static_cast<int>(i);
    }
    return std::nullopt;
}

int BaseCache::getVictimLRU(Address set) const {
    // Track {way_index, lru_counter} for victim selection
    using WayIndex = int;
    using LruCounter = uint32_t;
    auto [wayIndex, lruCounter] = std::pair<WayIndex, LruCounter>{0, 0};
    
    for (std::size_t currentWay = 0; currentWay < params_.assoc_; ++currentWay) {
        const auto& currentBlock = cache_[set + currentWay];
        
        // Invalid blocks are always preferred victims
        if (!isValidBlock(currentBlock)) {
            return static_cast<int>(currentWay);
        }
        
        // Extract the LRU counter bits from the block's metadata
        const LruCounter currentLruCounter = static_cast<LruCounter>(currentBlock.extraBits_ & g_lruMask);
        if (currentLruCounter > lruCounter) {
            wayIndex = static_cast<int>(currentWay);
            lruCounter = currentLruCounter;
        }
    }
    
    return wayIndex;
}

void BaseCache::updateLRU(int set, int way) {
    const Address targetLru = cache_[set + way].extraBits_ & g_lruMask;

    // Increment LRU for all blocks with lower or equal LRU value
    for (std::size_t wayIndex = 0; wayIndex < params_.assoc_; ++wayIndex) {
        const Address currentLru = cache_[set + wayIndex].extraBits_ & g_lruMask;
        if (currentLru <= targetLru && currentLru < g_lruMask) {
            cache_[set + wayIndex].extraBits_ += 1;
        }
    }
    
    // Reset the accessed block to most recently used (LRU = 0)
    cache_[set + way].extraBits_ &= ~g_lruMask;
}

Address BaseCache::handleCacheEviction(CacheBlock& block, [[maybe_unused]] Address addr) {
	setValid(block);
	return isDirtyBlock(block) ? block.addr_ : addr;
}

void BaseCache::checkHit(bool hit) {
	if (hit) {
		++stats_.hits_;
	}
	else {
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

Address BaseCache::makeMask(int start, int length) const {
	if (length == 0) return 0;
	if (start + length >= sizeof(Address) * 8) return ~0UL << start;
	return ( ( 1UL << length ) - 1 ) << start;
}

//Asccessor and Mutator Functions for dirty and valid bits
bool BaseCache::isDirtyBlock(const CacheBlock& block) const {
    return block.extraBits_ & g_dirtyMask;
}

void BaseCache::setDirty(CacheBlock& block) {
    block.extraBits_ |= g_dirtyMask;
}

void BaseCache::clearDirty(CacheBlock& block) {
    block.extraBits_ &= ~g_dirtyMask;
}

bool BaseCache::isValidBlock(const CacheBlock& block) const {
    return block.extraBits_ & g_validMask;
}

void BaseCache::setValid(CacheBlock& block) {
    block.extraBits_ |= g_validMask;
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





