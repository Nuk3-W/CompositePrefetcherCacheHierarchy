#pragma once

#include <cstdint>
#include <variant>
#include <functional>

using Address = uint32_t;

// Forward declaration
class CacheBlock;

struct Hit { std::reference_wrapper<CacheBlock> block; };
struct Miss { std::reference_wrapper<CacheBlock> block; };
struct Evict { std::reference_wrapper<CacheBlock> block; };

using AccessResult = std::variant<Hit, Miss, Evict>;

enum class AccessType {
    Read,
    Write
};

struct BitMasks {
    uint32_t tagBits_{};
    uint32_t setBits_{};
};

