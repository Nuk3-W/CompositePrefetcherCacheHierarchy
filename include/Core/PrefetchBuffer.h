#pragma once
#include "Core/Types.h"
#include "Core/CacheBlock.h"
#include "Utils/BitUtils.h"
#include <cmath>
#include <functional>

class PrefetchBuffer {
public:
	explicit PrefetchBuffer(uint32_t blockSize) {
		int blockBits = static_cast<int>(std::log2(blockSize));
		constexpr int addressBits = sizeof(Address) * 8;
		blockMask_ = Utils::makeMask(blockBits, addressBits - blockBits);
	}

	AccessResult read(Address addr) {
		Address blockAddr = addr & blockMask_;
		if (prefetchBlock_.isValid() && prefetchBlock_.getAddress() == blockAddr) {
			return Hit{std::ref(prefetchBlock_)};
		}
		return Miss{std::ref(prefetchBlock_)};
	}

	std::reference_wrapper<CacheBlock> offer(Address addr, uint32_t strategy) {
		Address blockAddr = addr & blockMask_;
		prefetchBlock_.initialize(blockAddr, AccessType::Read);
		currentStrategy_ = strategy;
		return std::ref(prefetchBlock_);
	}

	std::reference_wrapper<CacheBlock> getBlock() { return std::ref(prefetchBlock_); }
	
	uint32_t getCurrentStrategy() const { return currentStrategy_; }

private:
	CacheBlock prefetchBlock_{};
	Address blockMask_{};
	uint32_t currentStrategy_{0};
};


