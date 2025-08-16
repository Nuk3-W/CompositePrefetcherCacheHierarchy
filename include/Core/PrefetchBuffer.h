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

	std::reference_wrapper<CacheBlock> offer(Address addr) {
		Address blockAddr = addr & blockMask_;
		prefetchBlock_.initialize(blockAddr, AccessType::Read);
		return std::ref(prefetchBlock_);
	}

	std::reference_wrapper<CacheBlock> getBlock() { return std::ref(prefetchBlock_); }

private:
	CacheBlock prefetchBlock_{};
	Address blockMask_{};
};


