#pragma once

#include <cstdint>
#include <variant>

#include "Utils/BitUtils.h"
#include "Config/Constants.h"
#include "Core/CacheBlock.h"

using Address = uint32_t;

struct Hit { CacheBlock& block; };
struct Miss { CacheBlock& block; };
struct Evict { CacheBlock& block; };

using AccessResult = std::variant<Hit, Miss, Evict>;

enum class AccessType {
    Read,
    Write
};

struct BitMasks {
    uint32_t tagBits_{};
    uint32_t setBits_{};
};

