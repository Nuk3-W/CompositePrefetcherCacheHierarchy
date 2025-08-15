#pragma once

#include "Core/CacheBlock.h"
#include <cstdint>

class SuperBlock : public CacheBlock {
public:
	SuperBlock() = default;

	void resetHits() { hits_ = 0; }
	void incrementHits() { ++hits_; }
	uint32_t getHits() const { return hits_; }

private:
	uint32_t hits_{};
};